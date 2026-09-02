#include <qanvas/widgets/button.hpp>

// std
#include <algorithm>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/renderer.hpp>

// events
#include <qanvas/events/event.hpp>
#include <qanvas/events/widget_event.hpp>
#include <qanvas/events/input_event.hpp>

using namespace Qore::Qanvas;

Button::Button() {}

Size Button::compute() const {
    Size s{0, 0};
    for (Widget* c : children()) {
        Size cm = c->minimumSize();
        s.w = std::max(s.w, cm.w);
        s.h = std::max(s.h, cm.h);
    }
    return s;
}

void Button::arrange() {
    if (m_arranging) return;
    m_arranging = true;
    for (Widget* c : children()) {
        c->setRect(ContentRect{0, 0, rect().w, rect().h});
    }
    m_arranging = false;
}

void Button::setIdleColor(Color c) {
    m_idleBackground.setColor(c);
    requestRedraw();
}
void Button::setIdleImage(Image* image, FitMode fit) {
    m_idleBackground.setImage(image);
    m_idleBackground.setFit(fit);
    requestRedraw();
}
void Button::setIdleBackground(const Background& bg) {
    m_idleBackground = bg;
    requestRedraw();
}

void Button::setSelectedColor(Color c) {
    m_selectedBackground.setColor(c);
    requestRedraw();
}
void Button::setSelectedImage(Image* image, FitMode fit) {
    m_selectedBackground.setImage(image);
    m_selectedBackground.setFit(fit);
    requestRedraw();
}
void Button::setSelectedBackground(const Background& bg) {
    m_selectedBackground = bg;
    requestRedraw();
}

void Button::setActivatedColor(Color c) {
    m_activatedBackground.setColor(c);
    requestRedraw();
}
void Button::setActivatedImage(Image* image, FitMode fit) {
    m_activatedBackground.setImage(image);
    m_activatedBackground.setFit(fit);
    requestRedraw();
}
void Button::setActivatedBackground(const Background& bg) {
    m_activatedBackground = bg;
    requestRedraw();
}

void Button::setDisabledColor(Color c) {
    m_disabledBackground.setColor(c);
    requestRedraw();
}
void Button::setDisabledImage(Image* image, FitMode fit) {
    m_disabledBackground.setImage(image);
    m_disabledBackground.setFit(fit);
    requestRedraw();
}
void Button::setDisabledBackground(const Background& bg) {
    m_disabledBackground = bg;
    requestRedraw();
}

void Button::setCallback(std::function<void()> cb) {
    m_onActivated = std::move(cb);
}

bool Button::isFocusStop() const {
    return m_state != ButtonState::DISABLED;
}

void Button::draw(Canvas* canvas, ContentRect canvasSpaceRect) {
    const Background* bg;
    switch (m_state) {
        case ButtonState::IDLE:
            bg = &m_idleBackground;
            break;
        case ButtonState::SELECTED:
            bg = &m_selectedBackground;
            break;
        case ButtonState::ACTIVATED:
            bg = &m_activatedBackground;
            break;
        case ButtonState::DISABLED:
            bg = &m_disabledBackground;
            break;
    }
    Renderer::drawBackground(canvas, canvasSpaceRect, *bg);
}

bool Button::handle(const Event& e) {
    if (e.type() == EventType::WIDGET) {
        const auto& we = static_cast<const WidgetEvent&>(e);
        if (we.action == WidgetAction::FOCUS_GAINED) {
            m_state = ButtonState::SELECTED;
            requestRedraw();
            return true;
        }
        if (we.action == WidgetAction::FOCUS_LOST) {
            m_state = ButtonState::IDLE;
            requestRedraw();
            return true;
        }
        return false;
    }

    if (e.type() == EventType::INPUT) {
        const auto& ie = static_cast<const InputEvent&>(e);
        if (ie.action == InputAction::BUTTON_PRESS) {
            m_state = ButtonState::ACTIVATED;
            requestRedraw();
            return true;
        }
        if (ie.action == InputAction::BUTTON_RELEASE) {
            bool wasPressed = (m_state == ButtonState::ACTIVATED);
            m_state = ButtonState::SELECTED;
            requestRedraw();
            if (wasPressed && m_onActivated) m_onActivated();
            return true;
        }
    }
    return false;
}