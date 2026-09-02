#include <qanvas/widgets/center_container.hpp>

// std
#include <algorithm>

using namespace Qore::Qanvas;

Size CenterContainer::compute() const {
    uint16_t mw = 0, mh = 0;
    for (Widget* c : children()) {
        if (!c->visible()) continue;
        Size cm = c->minimumSize();
        mw = std::max(mw, cm.w);
        mh = std::max(mh, cm.h);
    }
    return {mw, mh};
}

void CenterContainer::arrange() {
    ContentRect r = rect();
    for (Widget* c : children()) {
        if (!c->visible()) continue;
        Size cm = c->minimumSize();
        int16_t cx = (int16_t)(r.w > cm.w ? (r.w - cm.w) / 2 : 0);
        int16_t cy = (int16_t)(r.h > cm.h ? (r.h - cm.h) / 2 : 0);
        c->setRect({cx, cy, (int16_t)cm.w, (int16_t)cm.h});
    }
}