#include <qanvas/widgets/keyboard.hpp>

// draw
#include <qanvas/draw/compositor.hpp>
#include <qanvas/draw/renderer.hpp>

// widgets
#include <qanvas/widgets/navigator.hpp>

using namespace Qore::Qanvas;

Keyboard::Keyboard(Compositor* compositor, Device* device, PhysicalRect area, Navigator* navigator, Widget* appRoot, int z, ScrollSuspendPolicy scrollPolicy) : Overlay(compositor, device, area, z, scrollPolicy), m_navigator(navigator), m_appRoot(appRoot) {}

void Keyboard::open(Widget* requester) {
    m_requester = requester;

    reset();
    show();

    if (m_navigator) m_navigator->rebuild(this);
}

void Keyboard::close() {
    hide();
    reset();

    if (m_navigator && m_appRoot) m_navigator->rebuild(m_appRoot, m_requester);

    if (m_onDone) m_onDone();
    m_requester = nullptr;
}

void Keyboard::setOnCharacter(std::function<void(char)> cb) {
    m_onCharacter = std::move(cb);
}
void Keyboard::setOnBackspace(std::function<void()> cb) {
    m_onBackspace = std::move(cb);
}
void Keyboard::setOnDone(std::function<void()> cb) {
    m_onDone = std::move(cb);
}

void Keyboard::setBackgroundColor(Color c) {
    m_background.setColor(c);
    requestRedraw();
}
void Keyboard::setBackgroundImage(Image* image, FitMode fit) {
    m_background.setImage(image);
    m_background.setFit(fit);
    requestRedraw();
}
void Keyboard::setBackground(const Background& background) {
    m_background = background;
    requestRedraw();
}


void Keyboard::draw(Canvas* canvas, ContentRect r) {
    Renderer::drawBackground(canvas, r, m_background);
}

void Keyboard::reset() {}

void Keyboard::emitCharacter(char c) {
    if (m_onCharacter) m_onCharacter(c);
}
void Keyboard::emitBackspace() {
    if (m_onBackspace) m_onBackspace();
}
void Keyboard::emitDone() {
    close();
}