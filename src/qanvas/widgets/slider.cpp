#include <qanvas/widgets/slider.hpp>

// std
#include <algorithm>

// events
#include <qanvas/events/event.hpp>
#include <qanvas/events/input_event.hpp>
#include <qanvas/events/widget_event.hpp>

// draw
#include <qanvas/draw/renderer.hpp>

using namespace Qore::Qanvas;

Slider::Slider(int64_t minValue, int64_t maxValue, int64_t step) : m_min(minValue), m_max(maxValue), m_step(step), m_value(minValue) {}

void Slider::setValue(int64_t v) {
    int64_t clamped = std::clamp(v, m_min, m_max);
    if (clamped == m_value) return;
    m_value = clamped;
    requestRedraw();
}

int64_t Slider::value() const {
    return m_value;
}

void Slider::setOnChanged(std::function<void(int64_t)> cb) {
    m_onChanged = std::move(cb);
}

void Slider::setTrackColor(Color c) {
    m_trackColor = c;
    requestRedraw();
}
void Slider::setFillColor(Color c) {
    m_fillColor = c;
    requestRedraw();
}
void Slider::setHandleColor(Color c) {
    m_handleColor = c;
    requestRedraw();
}
void Slider::setActiveHandleColor(Color c) {
    m_activeHandleColor = c;
    requestRedraw();
}

bool Slider::isFocusStop() const {
    return true;
}

void Slider::adjust(int64_t delta) {
    int64_t before = m_value;
    setValue(m_value + delta);
    if (m_value != before && m_onChanged) m_onChanged(m_value);
}

bool Slider::handle(const Event& e) {
    if (e.type() == EventType::WIDGET) {
        const auto& we = static_cast<const WidgetEvent&>(e);
        if (we.action == WidgetAction::FOCUS_GAINED) {
            m_focused = true;
            requestRedraw();
            return true;
        }
        if (we.action == WidgetAction::FOCUS_LOST) {
            m_focused = false;
            m_active = false;
            requestRedraw();
            return true;
        }
        return false;
    }

    if (e.type() == EventType::INPUT) {
        const auto& ie = static_cast<const InputEvent&>(e);

        if (ie.action == InputAction::BUTTON_PRESS) {
            m_active = !m_active;
            requestRedraw();
            return true;
        }

        if (!m_active) return false;

        if (ie.action == InputAction::ROTATE_CLOCKWISE) { adjust(m_step * ie.magnitude); return true; }
        if (ie.action == InputAction::ROTATE_ANTICLOCKWISE) { adjust(-m_step * ie.magnitude); return true; }
    }
    return false;
}

void Slider::draw(Canvas* canvas, ContentRect r) {
    Renderer::drawRect(canvas, r, m_trackColor);

    int64_t range = m_max - m_min;
    int16_t fillWidth = range > 0 ? (int16_t)(((int64_t)r.w * (m_value - m_min)) / range) : 0;
    Renderer::drawRect(canvas, ContentRect{r.x, r.y, fillWidth, r.h}, m_fillColor);

    if (m_focused) {
        int16_t handleW = 4;
        int16_t handleX = std::min((int16_t)(fillWidth - handleW / 2), (int16_t)(r.w - handleW));
        handleX = std::max(handleX, (int16_t)0);
        Color hc = m_active ? m_activeHandleColor : m_handleColor;
        Renderer::drawRect(canvas, ContentRect{(int16_t)(r.x + handleX), r.y, handleW, r.h}, hc);
    }
}