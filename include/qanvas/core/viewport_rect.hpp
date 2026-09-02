#ifndef __QANVAS_VIEWPORT_RECT_HPP__
#define __QANVAS_VIEWPORT_RECT_HPP__

// std
#include <cstdint>

namespace Qore::Qanvas {
    struct ViewportRect {
        int16_t x;
        int16_t y;
        int16_t w;
        int16_t h;
    };
}

#endif