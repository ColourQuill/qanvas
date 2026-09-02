#ifndef __QANVAS_PADDING_HPP__
#define __QANVAS_PADDING_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/content_rect.hpp>

namespace Qore::Qanvas {
    struct Padding {
        int16_t left = 0;
        int16_t top = 0;
        int16_t right = 0;
        int16_t bottom = 0;

        static Padding uniform(int16_t v);
        ContentRect apply(ContentRect r) const;
    };
}

#endif