#include <qanvas/core/background.hpp>

using namespace Qore::Qanvas;

Background::Background(Color color) : m_color(color) {}
Background::Background(Image* image, FitMode fit, Color color) : m_color(color), m_image(image), m_fit(fit) {}

void Background::setColor(Color c) {
    m_color = c;
}
Color Background::color() const {
    return m_color;
}

void Background::setImage(Image* image) {
    m_image = image;
}
Image* Background::image() const {
    return m_image;
}

void Background::setFit(FitMode fit) {
    m_fit = fit;
}
FitMode Background::fit() const {
    return m_fit;
}