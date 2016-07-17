/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_BOX_LAYOUT_HPP
#define TGUI_BOX_LAYOUT_HPP

#include <TGUI/Widgets/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Abstract class for box layout containers
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BoxLayout : public Panel
    {
    public:

        typedef std::shared_ptr<BoxLayout> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const BoxLayout> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BoxLayout();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the layout
        ///
        /// @param size  The new size of the layout
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setSize(const Layout2d& size) override;
        using Transformable::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the font the child widgets
        ///
        /// @param font  The new font
        ///
        /// When you don't call this function then the font from the parent widget will be used
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setFont(const Font& font) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inserts a widget to the layout
        ///
        /// @param widget      Pointer to the widget you would like to add
        /// @param index       Index of the widget in the container
        /// @param widgetName  An identifier to access to the widget later
        ///
        /// The widget will have a ratio of 1.
        ///
        /// If the index is too high, the widget will simply be added at the end of the list.
        ///
        /// @return False if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool insert(std::size_t index, const tgui::Widget::Ptr& widget, const sf::String& widgetName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a widget at the end of the layout
        ///
        /// @param widget      Pointer to the widget you would like to add
        /// @param widgetName  An identifier to access to the widget later
        ///
        /// The widget will have a ratio of 1.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void add(const tgui::Widget::Ptr& widget, const sf::String& widgetName = "") override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a space at the end of the layout and immediately set its ratio
        ///
        /// @param ratio  Ratio for the space
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void addSpace(float ratio = 1);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inserts a space to the layout and immediately set its ratio
        ///
        /// @param index  Index of the space in the container
        /// @param ratio  Ratio for the space
        ///
        /// If the index is too high, the space will simply be added at the end of the list.
        ///
        /// @return False if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool insertSpace(std::size_t index, float ratio = 1);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a single widget that was added to the container
        ///
        /// @param widget  Pointer to the widget to remove
        ///
        /// @return True if widget is removed, false if widget was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool remove(const tgui::Widget::Ptr& widget) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a single widget that was added to the container
        ///
        /// @param index  Index in the layout of the widget to remove
        ///
        /// @return False if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool remove(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the widget at the given index in the layout
        ///
        /// @param index  Index of the widget in the layout
        ///
        /// @return Widget of given index, or nullptr if index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr get(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the ratio of a widget
        ///
        /// The ratio is the size that will have a widget relatively to others. By default, the ratio is equal to 1.
        ///
        /// So setting a ratio to 2 means that the widget will be 2 times larger than widgets with a ratio equal to 1.
        ///
        /// @param widget  Pointer to the widget
        /// @param ratio   New ratio to set to the widget
        ///
        /// @return True if the ratio was changed, false if the widget was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setRatio(const Widget::Ptr& widget, float ratio);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the ratio of a widget
        /// The ratio is the size that will have a widget relatively to others. By default, the ratio is equal to 1.
        ///
        /// So setting a ratio to 2 means that the widget will be 2 times larger than widgets with a ratio equal to 1.
        ///
        /// @param index  Index of the widget in the layout
        /// @param ratio  New ratio to set to the widget
        ///
        /// If the index was too high then no change will be made
        ///
        /// @return False if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setRatio(std::size_t index, float ratio);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the ratio of a widget
        ///
        /// The ratio is the size that will have a widget relatively to others. By default, the ratio is equal to 1.
        ///
        /// So setting a ratio to 2 means that the widget will be 2 times larger than widgets with a ratio equal to 1.
        ///
        /// @param widget  Pointer to the widget
        ///
        /// @return Ratio given to the widget
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getRatio(const Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the ratio of a widget
        ///
        /// The ratio is the size that will have a widget relatively to others. By default, the ratio is equal to 1.
        ///
        /// So setting a ratio to 2 means that the widget will be 2 times larger than widgets with a ratio equal to 1.
        ///
        /// @param index  Index of the widget in the layout
        ///
        /// @return Ratio given to the widget or 0 if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getRatio(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the size of a widget to a constant value
        ///
        /// Setting a fixed size cancels the effect of the ratio.
        /// If you want to get a variable size again for a widget, set the fixed size to 0.
        ///
        /// @param widget Pointer to the widget
        /// @param size   New fixed size of the widget
        ///
        /// @return False if the widget was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setFixedSize(const Widget::Ptr& widget, float size);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the size of a widget to a constant value
        ///
        /// Setting a fixed size cancels the effect of the ratio.
        /// If you want to get a variable size again for a widget, set the fixed size to 0.
        ///
        /// @param index  Index of the widget in the layout
        /// @param size   New fixed size of the widget
        ///
        /// @return False if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setFixedSize(std::size_t index, float size);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the size of a widget when it has a constant value
        ///
        /// Having a fixed size cancels the effect of the ratio.
        /// When 0 is returned then the size of the widget is variable and determined by the ratio.
        ///
        /// @param widget Pointer to the widget
        ///
        /// @return Fixed size of the widget or 0 if size is variable
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getFixedSize(const Widget::Ptr& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the size of a widget when it has a constant value
        ///
        /// Having a fixed size cancels the effect of the ratio.
        /// When 0 is returned then the size of the widget is variable and determined by the ratio.
        ///
        /// @param index  Index of the widget in the layout
        ///
        /// @return Fixed size of the widget or 0 if size is variable or index is too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getFixedSize(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all widgets that were added to the container
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllWidgets() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Repositions all the widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void updateWidgetPositions() = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::vector<float> m_widgetsRatio;              ///< The ratio of each widget
        std::vector<float> m_widgetsFixedSizes;         ///< The fixed size of each widget. 0 means a variable size


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BOX_LAYOUT_HPP
