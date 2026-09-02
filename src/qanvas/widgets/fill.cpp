#include <qanvas/widgets/fill.hpp>

// std
#include <algorithm>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/renderer.hpp>

using namespace Qore::Qanvas;

Fill::Fill(Color color) : m_background(color) {}
Fill::Fill(const Background& background) : m_background(background) {}

void Fill::setColor(Color color) {
    m_background.setColor(color);
    requestRedraw();
}
Color Fill::color() const {
    return m_background.color();
}

void Fill::setImage(Image* image, FitMode fit) {
    m_background.setImage(image);
    m_background.setFit(fit);
    requestRedraw();
}
void Fill::setBackground(const Background& background) {
    m_background = background;
    requestRedraw();
}
const Background& Fill::background() const {
    return m_background;
}

void Fill::draw(Canvas* canvas, ContentRect canvasSpaceRect) {
    Renderer::drawBackground(canvas, canvasSpaceRect, m_background);
}