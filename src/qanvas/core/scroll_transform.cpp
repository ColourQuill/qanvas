#include <qanvas/core/scroll_transform.hpp>

// std
#include <algorithm>

using namespace Qore::Qanvas;

ScrollTransform::ScrollTransform(uint16_t contentHeight, uint16_t viewportHeight) : m_contentHeight(contentHeight), m_viewportHeight(viewportHeight) {}

size_t ScrollTransform::map(ContentRect content, ViewportRect out[1]) const {
    int64_t winStart = m_offset;
    int64_t winEnd = m_offset + m_viewportHeight;
    int64_t rectStart = content.y;
    int64_t rectEnd = content.y + content.h;

    int64_t clippedStart = std::max(winStart, rectStart);
    int64_t clippedEnd = std::min(winEnd, rectEnd);
    if (clippedEnd <= clippedStart) return 0;

    out[0] = ViewportRect{
        content.x,
        static_cast<int16_t>(clippedStart - winStart),
        content.w,
        static_cast<int16_t>(clippedEnd - clippedStart)
    };
    return 1;
}
bool ScrollTransform::consumeInvalidated() {
    bool v = m_invalidated;
    m_invalidated = false;
    return v;
}
int64_t ScrollTransform::contentRowForViewportRow(int16_t viewportRow) const {
    return m_offset + viewportRow;
}

void ScrollTransform::scrollTo(int64_t contentPosition, ScrollDirection dir) {
    if (dir != m_direction) m_invalidated = true;
    m_direction = dir;
    m_offset = std::clamp<int64_t>(contentPosition, 0, maxOffset());
}
void ScrollTransform::scrollBy(int64_t delta) {
    ScrollDirection dir = delta < 0 ? ScrollDirection::UP : delta > 0 ? ScrollDirection::DOWN : m_direction;
    scrollTo(m_offset + delta, dir);
}

int64_t ScrollTransform::offset() const {
    return m_offset;
}
int64_t ScrollTransform::maxOffset() const {
    return std::max<int64_t>(0, (int64_t)m_contentHeight - (int64_t)m_viewportHeight);
}
uint16_t ScrollTransform::viewportHeight() const {
    return m_viewportHeight;
}
ScrollDirection ScrollTransform::direction() const {
    return m_direction;
}

bool ScrollTransform::offsetChanged() const {
    return m_offset != m_lastSynced;
}
void ScrollTransform::markSynced() {
    m_lastSynced = m_offset;
}