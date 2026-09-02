#ifndef __QANVAS_COORD_TRANSFORM_HPP__
#define __QANVAS_COORD_TRANSFORM_HPP__

// std
#include <cstddef>

// core
#include <qanvas/core/content_rect.hpp>
#include <qanvas/core/viewport_rect.hpp>

namespace Qore::Qanvas {
    class CoordTransform {
        public:
            virtual ~CoordTransform() = default;
            virtual size_t map(ContentRect content, ViewportRect out[1]) const = 0;
            virtual bool consumeInvalidated() = 0;
            virtual int64_t contentRowForViewportRow(int16_t viewportRow) const = 0;
    };
}

#endif