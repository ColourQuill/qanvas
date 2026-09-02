#include <qanvas/widgets/navigator.hpp>

// std
#include <algorithm>

// events
#include <qanvas/events/event.hpp>
#include <qanvas/events/input_event.hpp>
#include <qanvas/events/widget_event.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

using namespace Qore::Qanvas;

void Navigator::rebuild(Widget* root, Widget* preferredFocus) {
    m_stops.clear();
    if (root) Widget::collectFocusStops(root, &m_stops);

    if (preferredFocus && std::find(m_stops.begin(), m_stops.end(), preferredFocus) != m_stops.end()) {
        setFocus(preferredFocus, 1);
        return;
    }

    if (m_current && std::find(m_stops.begin(), m_stops.end(), m_current) == m_stops.end()) {
        Widget* old = m_current;
        m_current = nullptr;
        old->handle(WidgetEvent(WidgetAction::FOCUS_LOST));
    }

    if (!m_current && !m_stops.empty()) {
        setFocus(m_stops.front(), 1);
    }
}

void Navigator::dispatch(const Event& e) {
    if (m_current && m_current->handle(e)) return;

    if (e.type() == EventType::INPUT) {
        const auto& ie = static_cast<const InputEvent&>(e);
        if (ie.action == InputAction::ROTATE_CLOCKWISE) {
            moveFocus(1);
        } else if (ie.action == InputAction::ROTATE_ANTICLOCKWISE) {
            moveFocus(-1);
        }
    }
}
Widget* Navigator::current() const {
    return m_current;
}

void Navigator::collect(Widget* w) {
    if (!w->visible()) return;
    if (w->isFocusStop()) {
        m_stops.push_back(w);
        return;
    }
    for (Widget* c : w->children()) {
        collect(c);
    }
}
void Navigator::moveFocus(int direction) {
    if (m_stops.empty()) return;

    auto it = std::find(m_stops.begin(), m_stops.end(), m_current);
    if (it == m_stops.end()) {
        setFocus(m_stops.front(), 1);
        return;
    }

    size_t idx = (size_t)std::distance(m_stops.begin(), it);
    size_t count = m_stops.size();

    if (direction > 0) {
        if (idx + 1 >= count) return;
        setFocus(m_stops[idx + 1], 1);
    } else {
        if (idx == 0) return;
        setFocus(m_stops[idx - 1], -1);
    }
}
void Navigator::setFocus(Widget* w, int direction) {
    if (m_current) m_current->handle(WidgetEvent(WidgetAction::FOCUS_LOST));
    m_current = w;
    if (m_current) {
        m_current->handle(WidgetEvent(WidgetAction::FOCUS_GAINED));
        m_current->onEnter(direction);
    }
}