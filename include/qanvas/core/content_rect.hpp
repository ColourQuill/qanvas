#ifndef __QANVAS_CONTENT_RECT_HPP__
#define __QANVAS_CONTENT_RECT_HPP__

// std
#include <cstdint>
#include <cmath>

namespace Qore::Qanvas {
    struct ContentRect {
        int16_t x;
        int16_t y;
        int16_t w;
        int16_t h;
    };

    inline bool rectsOverlap(ContentRect a, ContentRect b) {
        return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
    }
}

#endif