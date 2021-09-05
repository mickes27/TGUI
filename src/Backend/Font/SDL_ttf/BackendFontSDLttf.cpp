/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Backend/Font/SDL_ttf/BackendFontSDLttf.hpp>
#include <TGUI/Backend/Window/Backend.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendFontSDLttf::~BackendFontSDLttf()
    {
        for (auto& pair : m_fonts)
        {
            if (pair.second)
                TTF_CloseFont(pair.second);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSDLttf::loadFromMemory(std::unique_ptr<std::uint8_t[]> data, std::size_t sizeInBytes)
    {
        m_glyphs.clear();
        m_rows.clear();
        m_pixels = nullptr;
        m_texture = nullptr;
        m_textureUpToDate = false;
        m_textureSize = 0;
        m_nextRow = 3; // First 2 rows contain pixels for underlining

        constexpr unsigned int initialTextureSize = 128;
        m_pixels = MakeUniqueForOverwrite<std::uint8_t[]>(initialTextureSize * initialTextureSize * 4);

        std::uint8_t* pixelPtr = m_pixels.get();
        for (unsigned int y = 0; y < initialTextureSize; ++y)
        {
            for (unsigned int x = 0; x < initialTextureSize; ++x)
            {
                // Color is always white, alpha channel contains whether the pixel is empty of not
                *pixelPtr++ = 255;
                *pixelPtr++ = 255;
                *pixelPtr++ = 255;
                *pixelPtr++ = 0;
            }
        }

        // Reserve a 2x2 white square in the top left corner of the texture that we can use for drawing an underline
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int x = 0; x < 2; ++x)
                m_pixels[((initialTextureSize * y) + x) * 4 + 3] = 255;
        }

        m_textureSize = initialTextureSize;
        m_textureUpToDate = false;

        m_fileSize = sizeInBytes;
        m_fileContents = std::move(data);

        return (getInternalFont(getGlobalTextSize()) != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSDLttf::hasGlyph(char32_t codePoint) const
    {
        if (m_fonts.empty())
            return false;

        TGUI_ASSERT(m_fonts.begin()->second != nullptr, "BackendFontSDLttf::m_fonts shouldn't contain any nullptr fonts");

        // All fonts are the same, except for the character size, so we can use any of them to query for the character
#if SDL_TTF_MAJOR_VERSION > 2 || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION > 0) || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION == 0 && SDL_TTF_PATCHLEVEL > 15)
        return (TTF_GlyphIsProvided32(m_fonts.begin()->second, static_cast<std::uint32_t>(codePoint)) != 0);
#else
        return (TTF_GlyphIsProvided(m_fonts.begin()->second, static_cast<std::uint16_t>(codePoint)) != 0);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontSDLttf::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        const std::uint64_t glyphKey = constructGlyphKey(codePoint, characterSize, bold, outlineThickness);

        const auto it = m_glyphs.find(glyphKey);
        if (it != m_glyphs.end())
            return it->second;

        FontGlyph glyph;
        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return glyph;

        if (bold != m_lastBoldFlag[characterSize])
        {
            TTF_SetFontStyle(font, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
            m_lastBoldFlag[characterSize] = bold;
        }

        if (outlineThickness != m_lastOutlineThickness[characterSize])
        {
            TTF_SetFontOutline(font, static_cast<int>(outlineThickness));
            m_lastOutlineThickness[characterSize] = outlineThickness;
        }

        int minX;
        int maxX;
        int minY;
        int maxY;
        int advance;
#if SDL_TTF_MAJOR_VERSION > 2 || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION > 0) || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION == 0 && SDL_TTF_PATCHLEVEL > 15)
        if (TTF_GlyphMetrics32(font, static_cast<std::uint32_t>(codePoint), &minX, &maxX, &minY, &maxY, &advance) != 0)
#else
        if (TTF_GlyphMetrics(font, static_cast<std::uint16_t>(codePoint), &minX, &maxX, &minY, &maxY, &advance) != 0)
#endif
            return m_glyphs.insert({glyphKey, glyph}).first->second;

        glyph.advance = static_cast<float>(advance);
        glyph.bounds.left = static_cast<float>(minX);
        glyph.bounds.top = static_cast<float>(-maxY);
        glyph.bounds.width = static_cast<float>(maxX - minX);
        glyph.bounds.height = static_cast<float>(maxY - minY);

        // Glyphs such as spaces only have an advance and don't have texture
        if ((minX == maxX) || (minY == maxY))
            return m_glyphs.insert({glyphKey, glyph}).first->second;

#if SDL_TTF_MAJOR_VERSION > 2 || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION > 0) || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION == 0 && SDL_TTF_PATCHLEVEL > 15)
        SDL_Surface* surface = TTF_RenderGlyph32_Shaded(font, static_cast<std::uint32_t>(codePoint), {255, 255, 255, 255}, {0, 0, 0, 0});
#else
        SDL_Surface* surface = TTF_RenderGlyph_Shaded(font, static_cast<std::uint16_t>(codePoint), {255, 255, 255, 255}, {0, 0, 0, 0});
#endif
        if (surface)
        {
            const auto surfacePixels = static_cast<const std::uint8_t*>(surface->pixels);

            // When minX is negative, it seems like the pixels on the surface are located as if minX is 0
            const int surfaceLeft = std::max(0, minX);
            const int surfaceTop = TTF_FontAscent(font) - maxY;
            const int surfaceWidth = maxX - minX;
            const int surfaceHeight = maxY - minY;

            if ((surfaceLeft + surfaceWidth <= surface->w) && (surfaceTop + surfaceHeight <= surface->h))
            {
                // Find a good position for the new glyph into the texture.
                // We leave a small padding around characters, so that filtering doesn't pollute them with pixels from neighbors.
                const unsigned int padding = 2;
                glyph.textureRect = findAvailableGlyphRect(surfaceWidth + (2 * padding), surfaceHeight + (2 * padding));
                glyph.textureRect.left += padding;
                glyph.textureRect.top += padding;
                glyph.textureRect.width -= 2 * padding;
                glyph.textureRect.height -= 2 * padding;

                // Extract the glyph's pixels from the bitmap
                for (int y = surfaceTop; y < surfaceTop + surfaceHeight; ++y)
                {
                    for (int x = surfaceLeft; x < surfaceLeft + surfaceWidth; ++x)
                    {
                        const std::size_t index = (glyph.textureRect.left + x - surfaceLeft) + (glyph.textureRect.top + y - surfaceTop) * m_textureSize;
                        m_pixels[index * 4 + 3] = surfacePixels[(y * surface->pitch) + x];
                    }
                }

                // We will have to recreate the texture now that the pixels changed
                m_texture = nullptr;
            }

            SDL_FreeSurface(surface);
        }

        return m_glyphs.insert({glyphKey, glyph}).first->second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSDLttf::getKerning(char32_t first, char32_t second, unsigned int characterSize, bool bold)
    {
        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return 0;

        if (bold != m_lastBoldFlag[characterSize])
        {
            TTF_SetFontStyle(font, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
            m_lastBoldFlag[characterSize] = bold;
        }

#if SDL_TTF_MAJOR_VERSION > 2 || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION > 0) || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION == 0 && SDL_TTF_PATCHLEVEL > 15)
        const int kerning = TTF_GetFontKerningSizeGlyphs32(font, static_cast<std::uint32_t>(first), static_cast<std::uint32_t>(second));
#else
        const int kerning = TTF_GetFontKerningSizeGlyphs(font, static_cast<std::uint16_t>(first), static_cast<std::uint16_t>(second));
#endif

        return static_cast<float>(kerning);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSDLttf::getLineSpacing(unsigned int characterSize)
    {
        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return 0;

        return static_cast<float>(TTF_FontLineSkip(font));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSDLttf::getUnderlinePosition(unsigned int characterSize)
    {
        return static_cast<float>(getUnderlineInfo(characterSize).first);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSDLttf::getUnderlineThickness(unsigned int characterSize)
    {
        return static_cast<float>(getUnderlineInfo(characterSize).second);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendFontSDLttf::getTexture(unsigned int)
    {
        if (m_texture)
            return m_texture;

        m_texture = getBackend()->getRenderer()->createTexture();
        m_texture->loadTextureOnly({m_textureSize, m_textureSize}, m_pixels.get(), m_isSmooth);
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFontSDLttf::setSmooth(bool smooth)
    {
        BackendFont::setSmooth(smooth);
        if (m_texture)
            m_texture->setSmooth(m_isSmooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TTF_Font* BackendFontSDLttf::getInternalFont(unsigned int characterSize)
    {
        if (characterSize == 0)
            return nullptr;

        // Simply return the font if it already existed
        auto it = m_fonts.find(characterSize);
        if (it != m_fonts.end())
            return it->second;

        // Reload the font with a different character size when this was the first time the character size was requested
        if (!m_fileContents || (m_fileSize == 0))
            return nullptr;

        SDL_RWops* handle = SDL_RWFromConstMem(m_fileContents.get(), static_cast<int>(m_fileSize));
        if (!handle)
            return nullptr;

        auto font = TTF_OpenFontRW(handle, 1, characterSize);
        if (font)
        {
            m_fonts[characterSize] = font;
            m_lastBoldFlag[characterSize] = false;
        }

        return font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    TTF_Font* BackendFontSDLttf::loadInternalFont(unsigned int characterSize) const
    {
        if (!m_fileContents || (m_fileSize == 0))
            return nullptr;

        SDL_RWops* handle = SDL_RWFromConstMem(m_fileContents.get(), static_cast<int>(m_fileSize));
        if (!handle)
            return nullptr;

        return TTF_OpenFontRW(handle, 1, characterSize);
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<int, int> BackendFontSDLttf::getUnderlineInfo(unsigned int characterSize)
    {
        auto it = m_cachedUnderlineInfo.find(characterSize);
        if (it != m_cachedUnderlineInfo.end())
            return it->second;

        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return {0, 0};

        // Temporarily set the underline style
        const int oldFontStyle = TTF_GetFontStyle(font);
        TTF_SetFontStyle(font, TTF_STYLE_UNDERLINE);

        int offset = 0;
        int thickness = 0;

        // Draw a space with an underline. The underline will be white while the rest of the surface will be black
#if SDL_TTF_MAJOR_VERSION > 2 || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION > 0) || (SDL_TTF_MAJOR_VERSION == 2 && SDL_TTF_MINOR_VERSION == 0 && SDL_TTF_PATCHLEVEL > 15)
        SDL_Surface* surface = TTF_RenderGlyph32_Shaded(font, static_cast<std::uint32_t>(' '), {255, 255, 255, 255}, {0, 0, 0, 255});
#else
        SDL_Surface* surface = TTF_RenderGlyph_Shaded(font, static_cast<std::uint16_t>(' '), {255, 255, 255, 255}, {0, 0, 0, 255});
#endif
        if (surface)
        {
            // Find the offset and thickness of the underline.
            // Access a pixel at the horizontal center for each row, starting at the bottom.
            // Any non-black pixels are part of the underline (value may not be exactly 255).
            bool foundUnderline = false;
            for (int y = surface->h - 1; y >= 0; --y)
            {
                const bool isUnderlinePixel = (static_cast<const std::uint8_t*>(surface->pixels)[(y * surface->pitch) + (surface->w / 2)] != 0);

                if (!foundUnderline)
                {
                    if (isUnderlinePixel)
                        foundUnderline = true;
                    else
                        continue;
                }

                if (!isUnderlinePixel)
                {
                    offset = y + 1 - static_cast<int>(characterSize);
                    break;
                }

                ++thickness;
            }

            SDL_FreeSurface(surface);
        }

        // Restore the old style
        TTF_SetFontStyle(font, oldFontStyle);

        return m_cachedUnderlineInfo.emplace(characterSize, std::make_pair(offset, thickness)).first->second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    IntRect BackendFontSDLttf::findAvailableGlyphRect(unsigned int width, unsigned int height)
    {
        // Find the line that where the glyph fits well.
        // This is based on the sf::Font class in the SFML library. It might not be the most optimal method, but it is good enough for now.
        Row* bestRow = nullptr;
        float bestRatio = 0;
        for (auto& row : m_rows)
        {
            float ratio = static_cast<float>(height) / row.height;

            // Ignore rows that are either too small or too high
            if ((ratio < 0.7f) || (ratio > 1.f))
                continue;

            // Check if there's enough horizontal space left in the row
            if (width > m_textureSize - row.width)
                continue;

            // Make sure that this new row is the best found so far
            if (ratio < bestRatio)
                continue;

            // The current row passed all the tests: we can select it
            bestRow = &row;
            bestRatio = ratio;
        }

        // If we didn't find a matching row, create a new one (10% taller than the glyph)
        if (!bestRow)
        {
            // Check if the glyph can fit in the texture and resize the texture otherwise
            const unsigned int rowHeight = height + (height / 10);
            while ((m_nextRow + rowHeight >= m_textureSize) || (width >= m_textureSize))
            {
                /// TODO: Check if maximum texture size is reached and use multiple textures if there isn't enough space left

                // Copy existing pixels to the top left quadrant
                auto pixels = MakeUniqueForOverwrite<std::uint8_t[]>((m_textureSize * 2) * (m_textureSize * 2) * 4);
                for (unsigned int y = 0; y < m_textureSize; ++y)
                    std::memcpy(&pixels[y * (2 * m_textureSize) * 4], &m_pixels[y * m_textureSize * 4], m_textureSize * 4);

                // Top right quadrant and bottom halves are filled with empty values
                // Color is always white, alpha channel contains whether the pixel is empty of not
                for (unsigned int y = 0; y < m_textureSize; ++y)
                {
                    std::uint8_t* pixelPtr = &pixels[((y * (2 * m_textureSize)) + m_textureSize) * 4];
                    for (unsigned int i = 0; i < m_textureSize; ++i)
                    {
                        *pixelPtr++ = 255;
                        *pixelPtr++ = 255;
                        *pixelPtr++ = 255;
                        *pixelPtr++ = 0;
                    }
                }
                std::uint8_t* pixelPtr = &pixels[m_textureSize * (2 * m_textureSize) * 4];
                for (unsigned int i = 0; i < (2 * m_textureSize) * m_textureSize; ++i)
                {
                    *pixelPtr++ = 255;
                    *pixelPtr++ = 255;
                    *pixelPtr++ = 255;
                    *pixelPtr++ = 0;
                }

                m_pixels = std::move(pixels);
                m_textureSize *= 2;
            }

            // We can now create the new row
            m_rows.push_back({m_nextRow, rowHeight});
            m_nextRow += rowHeight;
            bestRow = &m_rows.back();

            m_textureUpToDate = false;
        }

        // Find the glyph's rectangle on the selected row
        IntRect rect(bestRow->width, bestRow->top, width, height);

        // Update the row informations
        bestRow->width += width;
        return rect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
