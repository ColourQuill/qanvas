#include <qanvas/widgets/margin_container.hpp>

// std
#include <algorithm>

namespace Qore::Qanvas {
    void MarginContainer::setMargin(uint16_t l, uint16_t t, uint16_t r, uint16_t b) {
        m_left = l; m_top = t; m_right = r; m_bottom = b;
        queueSort();
    }
    void MarginContainer::setMargin(uint16_t uniform) {
        setMargin(uniform, uniform, uniform, uniform);
    }

    Size MarginContainer::compute() const {
        uint16_t mw = 0, mh = 0;
        for (Widget* c : children()) {
            if (!c->visible()) continue;
            Size cm = c->minimumSize();
            mw = std::max(mw, cm.w);
            mh = std::max(mh, cm.h);
        }
        return {(uint16_t)(mw + m_left + m_right), (uint16_t)(mh + m_top + m_bottom)};
    }

    void MarginContainer::arrange() {
        ContentRect r = rect();
        ContentRect inner{
            (int16_t)m_left, (int16_t)m_top,
            (int16_t)(r.w > m_left + m_right ? r.w - m_left - m_right : 0),
            (int16_t)(r.h > m_top + m_bottom ? r.h - m_top - m_bottom : 0)
        };
        for (Widget* c : children()) {
            if (c->visible()) c->setRect(inner);
        }
    }
}