#include <qanvas/resources/text.hpp>

// std
#include <algorithm>
#include <sstream>
#include <cmath>
#include <iostream>

// resources
#include <qanvas/resources/font.hpp>

using namespace Qore::Qanvas;

static std::vector<std::string> splitLines(const std::string& content) {
    std::vector<std::string> out;
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line, '\n')) out.push_back(line);
    if (!content.empty() && content.back() == '\n') out.push_back("");
    if (out.empty()) out.push_back("");
    return out;
}

Text::Text(const std::string& content, Font* font, double size, Color color)
    : m_content(content), m_font(font), m_size(size), m_color(color) {
    reshape();
}

void Text::setContent(const std::string& content) {
    if (m_content == content) return;
    m_content = content;
    reshape();
}
void Text::setFont(Font* font) {
    if (m_font == font) return;
    m_font = font;
    reshape();
}
void Text::setSize(double size) {
    if (m_size == size) return;
    m_size = size;
    reshape();
}
void Text::setColor(Color color) {
    m_color = color;
}
void Text::setLineSpacing(double lineSpacing) {
    double v = std::max(0.1, lineSpacing);
    if (m_lineSpacing == v) return;
    m_lineSpacing = v;
    reshape();
}
void Text::setAlignment(HorizontalAlignment hAlign) {
    m_hAlign = hAlign;
}
void Text::setAlignment(VerticalAlignment vAlign) {
    m_vAlign = vAlign;
}
void Text::setMaxWidth(uint16_t w) {
    if (m_maxWidth == w) return;
    m_maxWidth = w;
    reshape();
}
void Text::setOverflow(TextOverflow mode) {
    if (m_overflow == mode) return;
    m_overflow = mode;
    reshape();
}

const std::string& Text::content() const {
    return m_content;
}
Font* Text::font() const {
    return m_font;
}
double Text::size() const {
    return m_size;
}
Color Text::color() const {
    return m_color;
}
double Text::lineSpacing() const {
    return m_lineSpacing;
}
HorizontalAlignment Text::hAlign() const {
    return m_hAlign;
}
VerticalAlignment Text::vAlign() const {
    return m_vAlign;
}
uint16_t Text::maxWidth() const {
    return m_maxWidth;
}
TextOverflow Text::overflow() const {
    return m_overflow;
}
uint16_t Text::width() const {
    return m_width;
}
uint16_t Text::height() const {
    return m_height;
}
const std::vector<TextLine>& Text::lines() const {
    return m_lines;
}

void Text::reshape() {
    m_lines.clear();
    if (!m_font) { m_width = 0; m_height = 0; return; }

    std::vector<std::string> paragraphs = splitLines(m_content);

    for (const std::string& para : paragraphs) {
        if (m_overflow == TextOverflow::WRAP && m_maxWidth > 0) {
            std::istringstream words(para);
            std::string word, currentLine;
            while (words >> word) {
                std::string candidate = currentLine.empty() ? word : currentLine + " " + word;
                if (measure(candidate) > m_maxWidth && !currentLine.empty()) {
                    m_lines.push_back({currentLine, measure(currentLine)});
                    currentLine = word;
                } else {
                    currentLine = candidate;
                }
            }
            m_lines.push_back({currentLine, measure(currentLine)});
        } else {
            m_lines.push_back({para, measure(para)});
        }
    }
    if (m_lines.empty()) m_lines.push_back({"", 0});

    int maxLineW = 0;
    for (const auto& l : m_lines) maxLineW = std::max(maxLineW, (int)l.width);

    double lineHeight = m_font->getLineHeight() * m_size;
    int totalHeight = (int)std::ceil(m_lines.size() * lineHeight * m_lineSpacing);

    m_width = (m_overflow == TextOverflow::WRAP) ? (uint16_t)longestWord() : (uint16_t)maxLineW;
    if (m_maxWidth > 0 && m_width > m_maxWidth) {
        m_width = m_maxWidth;
    }
    m_height = (uint16_t)totalHeight;
}

uint32_t Text::measure(const std::string& line) const {
    double cursor = 0;
    for (size_t i = 0; i < line.size(); ++i) {
        unsigned char c = (unsigned char)line[i];

        if (c == ' ') {
            const qtxt::GlyphRecord* spaceGlyph = m_font->getGlyph(' ');
            cursor += spaceGlyph ? spaceGlyph->advance * m_size : m_size * 0.4;
            continue;
        }

        const qtxt::GlyphRecord* g = m_font->getGlyph(c);
        if (!g) { cursor += m_size * 0.4; continue; }

        cursor += g->advance * m_size;
        cursor += m_font->getLetterSpacingRatio() * m_size;

        if (i + 1 < line.size()) {
            unsigned char next = (unsigned char)line[i + 1];
            cursor += m_font->getKerning(c, next) * m_size;
        }
    }
    return (uint32_t)std::ceil(cursor);
}

uint32_t Text::longestWord() const {
    uint32_t longest = 0;
    std::istringstream words(m_content);
    std::string word;
    while (words >> word) longest = std::max(longest, measure(word));
    return longest;
}