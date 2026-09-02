#ifndef __QANVAS_WIDGET_EVENT_HPP__
#define __QANVAS_WIDGET_EVENT_HPP__

// events
#include <qanvas/events/event.hpp>

namespace Qore::Qanvas {
    enum class WidgetAction {
        NONE,
        FOCUS_GAINED,
        FOCUS_LOST,
        ACTIVATE
    };

    struct WidgetEvent : Event {
        WidgetAction action = WidgetAction::NONE;

        WidgetEvent(WidgetAction action = WidgetAction::NONE) : action(action) {}

        EventType type() const override { return EventType::WIDGET; }
    };
}

#endif