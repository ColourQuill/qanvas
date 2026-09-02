#ifndef __QANVAS_SCROLL_DIRECTION_HPP__
#define __QANVAS_SCROLL_DIRECTION_HPP__

// std
#include <cstdint>

namespace Qore::Qanvas {
    enum class ScrollDirection : uint8_t {
        DOWN,
        UP
    };
}

#endif