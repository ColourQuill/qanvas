#include <qanvas/draw/canvas_view.hpp>

// std
#include <algorithm>
#include <iostream>

// core
#include <qanvas/core/scroll_transform.hpp>

using namespace Qore::Qanvas;

CanvasView::CanvasView(Canvas* canvas, int16_t deviceOriginX, int16_t deviceOriginY, CoordTransform* transform) : m_canvas(canvas), m_deviceOriginX(deviceOriginX), m_fixedDeviceOriginY(deviceOriginY), m_transform(transform) {}

Canvas* CanvasView::canvas() {
    return m_canvas;
}
CoordTransform* CanvasView::transform() {
    return m_transform;
}

int16_t CanvasView::deviceOriginX() const {
    return m_deviceOriginX;
}
int16_t CanvasView::deviceOriginY() const {
    int16_t top; uint16_t rows;
    if (!resolveVisiblePhysicalWindow(&top, &rows)) {
        return m_fixedDeviceOriginY;
    }
    return top;
}

size_t CanvasView::mapToPhysical(ContentRect content, PhysicalRect out[1]) const {
    ViewportRect viewport[1];
    size_t n = m_transform->map(content, viewport);
    if (n == 0) return 0;

    out[0] = PhysicalRect{
        static_cast<int16_t>(viewport[0].x + m_deviceOriginX),
        static_cast<int16_t>(viewport[0].y + deviceOriginY()),
        viewport[0].w,
        viewport[0].h
    };
    return 1;
}

void CanvasView::setNestedParent(CanvasView* parent, ContentRect ownRectInParent) {
    m_nestedParent = parent;
    m_ownRectInParent = ownRectInParent;
}

void CanvasView::registerNestedView(const void* key, ContentRect rect, CanvasView* nestedView) {
    for (auto& entry : m_nestedViews) {
        if (entry.key == key) { entry.rect = rect; entry.view = nestedView; return; }
    }
    m_nestedViews.push_back({key, rect, nestedView});
}

void CanvasView::unregisterNestedView(const void* key) {
    m_nestedViews.erase( std::remove_if(m_nestedViews.begin(), m_nestedViews.end(), [key](const NestedEntry& e) { return e.key == key; }), m_nestedViews.end());
}

const uint16_t* CanvasView::sourceRowForContentRow(int16_t contentRow) const {
    for (const auto& entry : m_nestedViews) {
        if (contentRow >= entry.rect.y && contentRow < entry.rect.y + entry.rect.h) {
            int16_t localRow = contentRow - entry.rect.y;
            int64_t nestedContentRow = entry.view->transform()->contentRowForViewportRow(localRow);
            return entry.view->canvas()->row((uint16_t)nestedContentRow);
        }
    }
    return m_canvas->row((uint16_t)contentRow);
}

bool CanvasView::resolveVisiblePhysicalWindow(int16_t* outTop, uint16_t* outRows) const {
    if (!m_nestedParent) {
        *outTop = m_fixedDeviceOriginY;
        *outRows = ownDisplayHeight();
        return true;
    }

    int16_t parentTop; uint16_t parentRows;
    if (!m_nestedParent->resolveVisiblePhysicalWindow(&parentTop, &parentRows)) {
        return false;
    }

    auto* parentScroll = dynamic_cast<ScrollTransform*>(m_nestedParent->transform());
    int64_t parentContentTop = parentScroll ? parentScroll->offset() : 0;
    int64_t parentContentBottom = parentContentTop + parentRows;

    int64_t myTop = m_ownRectInParent.y;
    int64_t myBottom = m_ownRectInParent.y + m_ownRectInParent.h;
    int64_t visStart = std::max(myTop, parentContentTop);
    int64_t visEnd = std::min(myBottom, parentContentBottom);
    if (visEnd <= visStart) {
        return false;
    }

    *outTop = (int16_t)(parentTop + (visStart - parentContentTop));
    *outRows = (uint16_t)(visEnd - visStart);
    return true;
}

uint16_t CanvasView::ownDisplayHeight() const {
    if (auto* st = dynamic_cast<ScrollTransform*>(m_transform)) {
        return st->viewportHeight();
    }
    return m_canvas->height();
}

bool CanvasView::physicalToContentClip(PhysicalRect phys, ContentRect* out) const {
    int16_t top; uint16_t rows;
    if (!resolveVisiblePhysicalWindow(&top, &rows)) return false;

    int16_t vx0 = std::max(phys.x, m_deviceOriginX);
    int16_t vy0 = std::max(phys.y, top);
    int16_t vx1 = std::min<int16_t>((int16_t)(phys.x + phys.w), (int16_t)(m_deviceOriginX + (int16_t)m_canvas->width()));
    int16_t vy1 = std::min<int16_t>((int16_t)(phys.y + phys.h), (int16_t)(top + rows));
    if (vx1 <= vx0 || vy1 <= vy0) return false;

    int16_t viewportRow0 = (int16_t)(vy0 - top);
    int64_t contentY0 = m_transform->contentRowForViewportRow(viewportRow0);

    *out = ContentRect{
        (int16_t)(vx0 - m_deviceOriginX),
        (int16_t)contentY0,
        (int16_t)(vx1 - vx0),
        (int16_t)(vy1 - vy0)
    };
    return true;
}

bool CanvasView::physicalFootprint(PhysicalRect* out) const {
    int16_t top; uint16_t rows;
    if (!resolveVisiblePhysicalWindow(&top, &rows)) return false;
    *out = PhysicalRect{m_deviceOriginX, top, (int16_t)m_canvas->width(), (int16_t)rows};
    return true;
}