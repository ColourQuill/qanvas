#include <qanvas/core/identity_transform.hpp>

using namespace Qore::Qanvas;

size_t IdentityTransform::map(ContentRect content, ViewportRect out[1]) const {
    out[0] = ViewportRect{content.x, content.y, content.w, content.h};
    return 1;
}
bool IdentityTransform::consumeInvalidated() {
    return false;
}
int64_t IdentityTransform::contentRowForViewportRow(int16_t viewportRow) const {
    return viewportRow;
}