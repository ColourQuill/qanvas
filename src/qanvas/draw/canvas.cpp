#include <qanvas/draw/canvas.hpp>

// std
#include <cassert>
#include <iostream>

using namespace Qore::Qanvas;

Canvas::Canvas(Extent extent, CanvasUsage usage) : m_extent(extent), m_usage(usage), m_data((size_t)extent.w * extent.h) {}

uint16_t Canvas::width() const {
    return m_extent.w;
}
uint16_t Canvas::height() const {
    return m_extent.h;
}
Extent Canvas::extent() const {
    return m_extent;
}
CanvasUsage Canvas::usage() const {
    return m_usage;
}

uint16_t* Canvas::data() {
    return m_data.data();
}
const uint16_t* Canvas::data() const {
    return m_data.data();
}

uint16_t* Canvas::row(uint16_t y) {
    assert(y < m_extent.h);
    return m_data.data() + (size_t)y * m_extent.w;
}
const uint16_t* Canvas::row(uint16_t y) const {
    assert(y < m_extent.h);
    return m_data.data() + (size_t)y * m_extent.w;
}

void Canvas::markDirty(ContentRect rect) {
    if (rect.x < 0) { rect.w += rect.x; rect.x = 0; }
    if (rect.y < 0) { rect.h += rect.y; rect.y = 0; }
    if (rect.x + rect.w > m_extent.w) rect.w = m_extent.w - rect.x;
    if (rect.y + rect.h > m_extent.h) rect.h = m_extent.h - rect.y;
    if (rect.w <= 0 || rect.h <= 0) return;
    m_dirty.push_back(rect);
}

bool Canvas::takeDirty(ContentRect& out) {
    if (m_dirty.empty()) return false;
    out = m_dirty.back();
    m_dirty.pop_back();
    return true;
}

void Canvas::clearDirty() {
    m_dirty.clear();
}

bool Canvas::intersectsDirty(ContentRect rect) const {
    for (const ContentRect& d : m_dirty) {
        if (rectsOverlap(d, rect)) return true;
    }
    return false;
}

void Canvas::setClearColor(Color c) {
    m_clearColor = c;
}

void Canvas::clearDirtyRegions() {
    uint16_t packed = m_clearColor.pack();
    for (const ContentRect& r : m_dirty) {
        int16_t x0 = std::max<int16_t>(r.x, 0);
        int16_t y0 = std::max<int16_t>(r.y, 0);
        int16_t x1 = std::min<int16_t>((int16_t)(r.x + r.w), (int16_t)m_extent.w);
        int16_t y1 = std::min<int16_t>((int16_t)(r.y + r.h), (int16_t)m_extent.h);
        for (int16_t y = y0; y < y1; ++y) {
            uint16_t* rowPtr = row((uint16_t)y) + x0;
            std::fill(rowPtr, rowPtr + (x1 - x0), packed);
        }
    }
}