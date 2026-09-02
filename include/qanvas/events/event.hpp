#ifndef __QANVAS_EVENT_HPP__
#define __QANVAS_EVENT_HPP__

enum class EventType {
    INPUT,
    WIDGET
};

namespace Qore::Qanvas {
    struct Event {
        virtual ~Event() = default;
        virtual EventType type() const = 0;
    };
}

#endif