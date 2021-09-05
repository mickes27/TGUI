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


#ifndef TGUI_BACKEND_TEXTURE_HPP
#define TGUI_BACKEND_TEXTURE_HPP

#include <TGUI/Font.hpp>
#include <TGUI/String.hpp>
#include <memory>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for texture implementations that depend on the backend
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendTexture
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~BackendTexture() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from an array of 32-bits RGBA pixels
        ///
        /// @param size   Width and height of the image to create
        /// @param pixels Moved pointer to array of size.x*size.y*4 bytes with RGBA pixels, or nullptr to create an empty texture
        /// @param smooth Should the smooth filter be enabled or not?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool load(Vector2u size, std::unique_ptr<std::uint8_t[]> pixels, bool smooth);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from an array of 32-bits RGBA pixels, but don't take ownership of the pixels
        ///
        /// @param size   Width and height of the image to create
        /// @param pixels Pointer to array of size.x*size.y*4 bytes with RGBA pixels, or nullptr to create an empty texture
        /// @param smooth Should the smooth filter be enabled or not?
        ///
        /// @warning Unlike the load function, loadTextureOnly won't store the pixels and isTransparentPixel thus won't work
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the size of the entire image
        /// @return Texture size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_NEXT
        Vector2u getSize() const;
#else
        virtual Vector2u getSize() const;
#endif


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether the smooth filter is enabled or not
        ///
        /// @param smooth  True if smoothing should be enabled, false if it should be disabled
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSmooth(bool smooth);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the smooth filter is enabled or not
        ///
        /// @return True if smoothing is enabled, false if it is disabled
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_NEXT
        bool isSmooth() const;
#else
        virtual bool isSmooth() const;
#endif


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if a certain pixel is transparent
        ///
        /// @param pixel  Coordinate of the pixel
        ///
        /// @return True when the pixel is transparent, false when it is not
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isTransparentPixel(Vector2u pixel) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        Vector2u m_imageSize;
        std::unique_ptr<std::uint8_t[]> m_pixels;
        bool m_isSmooth = true;
    };

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    using BackendTextureBase TGUI_DEPRECATED("BackendTextureBase was renamed to BackendTexture") = BackendTexture;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_TEXTURE_HPP
