#ifndef __QANVAS_LABEL_HPP__
#define __QANVAS_LABEL_HPP__

// std
#include <string>

// core
#include <qanvas/core/color.hpp>

// resources
#include <qanvas/resources/text.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Font;

    class Label : public Widget {
        public:
            Label() = default;
            Label(const std::string& text, Font* font, double size, Color color = Color::WHITE);

            void setText(const std::string& text);
            void setFont(Font* font);
            void setSize(double size);
            void setColor(Color color);
            void setLineSpacing(double factor);
            void setOverflow(TextOverflow mode);

            void setTextQuiet(const std::string& t);

            void setRect(ContentRect r) override;

            Text& text();
            const Text& text() const;

            Size minimumSize() const override;
            void draw(Canvas* canvas, ContentRect canvasSpaceRect) override;
        private:
            Text m_text{};
    };
}

#endif