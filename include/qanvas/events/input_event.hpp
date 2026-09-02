#ifndef __QANVAS_INPUT_EVENT_HPP__
#define __QANVAS_INPUT_EVENT_HPP__

// std
#include <cstdint>

// events
#include <qanvas/events/event.hpp>

namespace Qore::Qanvas {
    enum class InputAction {
        NONE,
        ROTATE_CLOCKWISE,
        ROTATE_ANTICLOCKWISE,
        BUTTON_PRESS,
        BUTTON_RELEASE
    };

    struct InputEvent : Event {
        InputAction action = InputAction::NONE;
        int32_t magnitude = 1;
        int32_t deviceId = 0;

        InputEvent(InputAction action = InputAction::NONE, int32_t magnitude = 1) : action(action), magnitude(magnitude) {}

        EventType type() const override { return EventType::INPUT; }
    };
}

#endif