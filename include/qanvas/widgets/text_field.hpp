#ifndef __QANVAS_TEXT_FIELD_HPP__
#define __QANVAS_TEXT_FIELD_HPP__

// std
#include <string>
#include <functional>

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/padding.hpp>
#include <qanvas/core/background.hpp>
#include <qanvas/core/fit_mode.hpp>

// resources
#include <qanvas/resources/text.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Font;
    class Image;
    class Keyboard;
    class ScrollContainer;

    class TextField : public Widget {
        public:
            TextField(Keyboard* keyboard, Font* font, double size, Color color = Color::WHITE);

            const std::string& value() const;
            void setValue(const std::string& v);
            void setOnChanged(std::function<void(const std::string&)> cb);

            void setPlaceholder(const std::string& text);
            void setPlaceholderColor(Color c);

            void setPadding(Padding p);
            void setPadding(int16_t uniform);

            void setIdleColor(Color c);
            void setIdleImage(Image* image, FitMode fit = FitMode::COVER);
            void setIdleBackground(const Background& bg);

            void setSelectedColor(Color c);
            void setSelectedImage(Image* image, FitMode fit = FitMode::COVER);
            void setSelectedBackground(const Background& bg);

            void setEditingColor(Color c);
            void setEditingImage(Image* image, FitMode fit = FitMode::COVER);
            void setEditingBackground(const Background& bg);

            bool isFocusStop() const override;
            bool handle(const Event& e) override;
            void draw(Canvas* canvas, ContentRect r) override;
        private:
            void openKeyboard();
            void onKeyboardClosed();
            void appendChar(char c);
            void backspace();
            void resolveAncestors();

            Keyboard* m_keyboard;

            ScrollContainer* m_enclosingScroller = nullptr;

            std::string m_value;
            Text m_text;
            Text m_placeholderText;
            bool m_focused = false;
            bool m_editing = false;
            std::function<void(const std::string&)> m_onChanged;

            Padding m_padding = Padding::uniform(4);
            Background m_idleBackground{Color{45, 45, 48}};
            Background m_selectedBackground{Color{0, 122, 204}};
            Background m_editingBackground{Color{160, 60, 20}};
            Color m_placeholderColor{120, 120, 120};
    };
}

#endif