#include <qanvas/widgets/box_container.hpp>

// std
#include <algorithm>
#include <vector>
#include <iostream>

using namespace Qore::Qanvas;

BoxContainer::BoxContainer(BoxAxis axis, uint16_t separation) : m_axis(axis), m_separation(separation) {}

void BoxContainer::setSeparation(uint16_t px) {
    m_separation = px;
    queueSort();
}
uint16_t BoxContainer::separation() const { return m_separation; }

Size BoxContainer::compute() const {
    uint16_t mainTotal = 0;
    uint16_t crossMax = 0;
    int count = 0;
    for (Widget* c : children()) {
        if (!c->visible()) continue;
        Size cm = c->minimumSize();
        if (m_axis == BoxAxis::HORIZONTAL) {
            mainTotal += cm.w;
            crossMax = std::max(crossMax, cm.h);
        } else {
            mainTotal += cm.h;
            crossMax = std::max(crossMax, cm.w);
        }
        ++count;
    }
    if (count > 1) mainTotal += m_separation * (count - 1);

    return m_axis == BoxAxis::HORIZONTAL ? Size{mainTotal, crossMax} : Size{crossMax, mainTotal};
}

void BoxContainer::arrange() {
    ContentRect r = rect();

    std::vector<Widget*> visibleChildren;
    for (Widget* c : children()) if (c->visible()) visibleChildren.push_back(c);
    if (visibleChildren.empty()) return;

    uint16_t mainSize = (m_axis == BoxAxis::HORIZONTAL) ? r.w : r.h;
    uint16_t crossSize = (m_axis == BoxAxis::HORIZONTAL) ? r.h : r.w;

    uint16_t minTotal = 0;
    int expandCount = 0;
    for (Widget* c : visibleChildren) {
        Size cm = c->minimumSize();
        minTotal += (m_axis == BoxAxis::HORIZONTAL) ? cm.w : cm.h;
        SizeFlag mainFlags = (m_axis == BoxAxis::HORIZONTAL) ? c->hSizeFlags() : c->vSizeFlags();
        if (hasFlag(mainFlags, SizeFlag::EXPAND)) ++expandCount;
    }
    if (visibleChildren.size() > 1) minTotal += m_separation * (uint16_t)(visibleChildren.size() - 1);

    int extra = std::max(0, (int)mainSize - (int)minTotal);
    int perExpand = expandCount > 0 ? extra / expandCount : 0;
    int remainder = expandCount > 0 ? extra % expandCount : 0;

    int cursor = 0;
    for (Widget* c : visibleChildren) {
        Size cm = c->minimumSize();
        int mainDim = (m_axis == BoxAxis::HORIZONTAL) ? cm.w : cm.h;

        SizeFlag mainFlags = (m_axis == BoxAxis::HORIZONTAL) ? c->hSizeFlags() : c->vSizeFlags();
        if (hasFlag(mainFlags, SizeFlag::EXPAND)) {
            mainDim += perExpand;
            if (remainder > 0) { mainDim += 1; --remainder; }
        }

        int maxMainDim = (int)mainSize - cursor;
        if (mainDim > maxMainDim) mainDim = maxMainDim > 0 ? maxMainDim : 0;

        SizeFlag crossFlags = (m_axis == BoxAxis::HORIZONTAL) ? c->vSizeFlags() : c->hSizeFlags();
        int crossDim = hasFlag(crossFlags, SizeFlag::FILL) ? crossSize : (m_axis == BoxAxis::HORIZONTAL ? cm.h : cm.w);
        int crossPos = 0;
        if (!hasFlag(crossFlags, SizeFlag::FILL)) {
            if (hasFlag(crossFlags, SizeFlag::SHRINK_CENTER)) crossPos = (crossSize - crossDim) / 2;
            else if (hasFlag(crossFlags, SizeFlag::SHRINK_END)) crossPos = crossSize - crossDim;
        }

        ContentRect childRect;
        if (m_axis == BoxAxis::HORIZONTAL) {
            childRect = { (int16_t)cursor, (int16_t)crossPos, (int16_t)mainDim, (int16_t)crossDim };
        } else {
            childRect = { (int16_t)crossPos, (int16_t)cursor, (int16_t)crossDim, (int16_t)mainDim };
        }
        c->setRect(childRect);

        cursor += mainDim + m_separation;
    }
}