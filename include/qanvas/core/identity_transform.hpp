#ifndef __QANVAS_IDENTITY_TRANSFORM_HPP__
#define __QANVAS_IDENTITY_TRANSFORM_HPP__

// core
#include <qanvas/core/coord_transform.hpp>

namespace Qore::Qanvas {
    class IdentityTransform : public CoordTransform {
        public:
            size_t map(ContentRect content, ViewportRect out[1]) const override;
            bool consumeInvalidated() override;
            int64_t contentRowForViewportRow(int16_t viewportRow) const override;
    };
}

#endif