#include <qanvas/core/padding.hpp>

// std
#include <algorithm>

namespace Qore::Qanvas {
    Padding Padding::uniform(int16_t v) {
        return Padding{v, v, v, v};
    }

    ContentRect Padding::apply(ContentRect r) const {
        int16_t w = (int16_t)std::max<int16_t>(0, (int16_t)(r.w - left - right));
        int16_t h = (int16_t)std::max<int16_t>(0, (int16_t)(r.h - top - bottom));
        return ContentRect{
            (int16_t)(r.x + left),
            (int16_t)(r.y + top),
            w,
            h
        };
    }
}