#ifndef __QANVAS_CANVAS_HPP__
#define __QANVAS_CANVAS_HPP__

// std
#include <cstdint>
#include <vector>

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/extent.hpp>
#include <qanvas/core/content_rect.hpp>
#include <qanvas/core/canvas_usage.hpp>

namespace Qore::Qanvas {
    class Canvas {
        public:
            Canvas(Extent extent, CanvasUsage usage);

            uint16_t width() const;
            uint16_t height() const;
            Extent extent() const;
            CanvasUsage usage() const;

            uint16_t* data();
            const uint16_t* data() const;

            uint16_t* row(uint16_t y);
            const uint16_t* row(uint16_t y) const;

            void markDirty(ContentRect rect);
            bool takeDirty(ContentRect& out);
            void clearDirty();
            bool intersectsDirty(ContentRect rect) const;
            void setClearColor(Color c);
            void clearDirtyRegions();
        private:
            Extent m_extent;
            CanvasUsage m_usage;
            std::vector<uint16_t> m_data;
            std::vector<ContentRect> m_dirty;
            Color m_clearColor{0, 0, 0};
    };
}

#endif