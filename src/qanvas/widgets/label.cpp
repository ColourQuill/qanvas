#include <qanvas/widgets/label.hpp>
#include <qanvas/draw/renderer.hpp>
#include <algorithm>

using namespace Qore::Qanvas;

Label::Label(const std::string& text, Font* font, double size, Color color) : m_text(text, font, size, color) {}

void Label::setText(const std::string& t) {
    m_text.setContent(t);
    requestRedraw();
    queueSort();
}
void Label::setFont(Font* f) {
    m_text.setFont(f);
    requestRedraw();
    queueSort();
}
void Label::setSize(double s) {
    m_text.setSize(s);
    requestRedraw();
    queueSort();
}
void Label::setColor(Color c) {
    m_text.setColor(c);
    requestRedraw();
}
void Label::setLineSpacing(double f) {
    m_text.setLineSpacing(f);
    requestRedraw();
    queueSort();
}
void Label::setOverflow(TextOverflow mode) {
    m_text.setOverflow(mode);
    requestRedraw();
    queueSort();
}

void Label::setTextQuiet(const std::string& t) {
    m_text.setContent(t);
    requestRedraw();
}

void Label::setRect(ContentRect r) {
    uint16_t oldWidth = m_text.width();
    uint16_t oldHeight = m_text.height();

    m_text.setMaxWidth((uint16_t)r.w);
    Widget::setRect(r);

    if (m_text.width() != oldWidth || m_text.height() != oldHeight) {
        queueSort();
    }
}

Text& Label::text() {
    return m_text;
}
const Text& Label::text() const {
    return m_text;
}

Size Label::minimumSize() const {
    Size fromText = {m_text.width(), m_text.height()};
    Size custom = Widget::minimumSize();
    return {std::max(fromText.w, custom.w), std::max(fromText.h, custom.h)};
}

void Label::draw(Canvas* canvas, ContentRect canvasSpaceRect) {
    Renderer::drawText(canvas, m_text, canvasSpaceRect);
}