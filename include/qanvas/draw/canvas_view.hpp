#ifndef __QANVAS_CANVAS_VIEW_HPP__
#define __QANVAS_CANVAS_VIEW_HPP__

// std
#include <cstdint>
#include <vector>

// core
#include <qanvas/core/content_rect.hpp>
#include <qanvas/core/viewport_rect.hpp>
#include <qanvas/core/physical_rect.hpp>
#include <qanvas/core/coord_transform.hpp>

// draw
#include <qanvas/draw/canvas.hpp>

namespace Qore::Qanvas {
    class Widget;

    class CanvasView {
        public:
            CanvasView(Canvas* canvas, int16_t deviceOriginX, int16_t deviceOriginY, CoordTransform* transform);

            Canvas* canvas();
            CoordTransform* transform();

            int16_t deviceOriginX() const;
            int16_t deviceOriginY() const;

            size_t mapToPhysical(ContentRect content, PhysicalRect out[1]) const;

            void setNestedParent(CanvasView* parent, ContentRect ownRectInParent);
            void registerNestedView(const void* key, ContentRect rect, CanvasView* nestedView);
            void unregisterNestedView(const void* key);

            bool resolveVisiblePhysicalWindow(int16_t* outTop, uint16_t* outRows) const;
            const uint16_t* sourceRowForContentRow(int16_t contentRow) const;
            bool physicalToContentClip(PhysicalRect phys, ContentRect* out) const;
            bool physicalFootprint(PhysicalRect* out) const;
        private:
            struct NestedEntry {
                const void* key;
                ContentRect rect;
                CanvasView* view;
            };

            Canvas* m_canvas;
            int16_t m_deviceOriginX;
            int16_t m_fixedDeviceOriginY;
            CoordTransform* m_transform;

            CanvasView* m_nestedParent = nullptr;
            ContentRect m_ownRectInParent{0, 0, 0, 0};

            std::vector<NestedEntry> m_nestedViews;

            uint16_t ownDisplayHeight() const;
    };
}

#endif