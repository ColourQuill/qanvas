#ifndef __QANVAS_TEXT_HPP__
#define __QANVAS_TEXT_HPP__

// std
#include <cstdint>
#include <string>
#include <vector>

// core
#include <qanvas/core/color.hpp>

namespace Qore::Qanvas {
    class Font;

    enum class HorizontalAlignment {
        LEFT,
        CENTER,
        RIGHT
    };
    enum class VerticalAlignment {
        TOP,
        CENTER,
        BOTTOM
    };
    enum class TextOverflow {
        CLIP,
        WRAP
    };

    struct TextLine {
        std::string text;
        uint32_t width;
    };

    class Text {
        public:
            Text() = default;
            Text(const std::string& content, Font* font, double size, Color color);

            void setContent(const std::string& content);
            void setFont(Font* font);
            void setSize(double size);
            void setColor(Color color);
            void setLineSpacing(double lineSpacing);
            void setAlignment(HorizontalAlignment hAlign);
            void setAlignment(VerticalAlignment vAlign);
            void setMaxWidth(uint16_t w);
            void setOverflow(TextOverflow mode);

            const std::string& content() const;
            Font* font() const;
            double size() const;
            Color color() const;
            double lineSpacing() const;
            HorizontalAlignment hAlign() const;
            VerticalAlignment vAlign() const;
            uint16_t maxWidth() const;
            TextOverflow overflow() const;

            uint16_t width() const;
            uint16_t height() const;

            const std::vector<TextLine>& lines() const;
        private:
            std::string m_content;
            Font* m_font = nullptr;
            double m_size = 12.0;
            Color m_color = Color::WHITE;
            double m_lineSpacing = 1.0;
            HorizontalAlignment m_hAlign = HorizontalAlignment::LEFT;
            VerticalAlignment m_vAlign = VerticalAlignment::TOP;
            TextOverflow m_overflow = TextOverflow::CLIP;
            uint16_t m_maxWidth = 0;

            std::vector<TextLine> m_lines;
            uint16_t m_width = 0;
            uint16_t m_height = 0;

            void reshape();
            uint32_t measure(const std::string& line) const;
            uint32_t longestWord() const;
    };
}

#endif