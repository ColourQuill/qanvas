#ifndef __QANVAS_SCROLL_TRANSFORM_HPP__
#define __QANVAS_SCROLL_TRANSFORM_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/coord_transform.hpp>
#include <qanvas/core/scroll_direction.hpp>

namespace Qore::Qanvas {
    class ScrollTransform : public CoordTransform {
        public:
            ScrollTransform(uint16_t contentHeight, uint16_t viewportHeight);

            size_t map(ContentRect content, ViewportRect out[1]) const override;
            bool consumeInvalidated() override;
            int64_t contentRowForViewportRow(int16_t viewportRow) const override;

            void scrollTo(int64_t contentPosition, ScrollDirection dir);
            void scrollBy(int64_t delta);

            int64_t offset() const;
            int64_t maxOffset() const;
            uint16_t viewportHeight() const;
            ScrollDirection direction() const;

            bool offsetChanged() const;
            void markSynced();
        private:
            uint16_t m_contentHeight;
            uint16_t m_viewportHeight;
            int64_t m_offset = 0;
            int64_t m_lastSynced = 0;
            ScrollDirection m_direction = ScrollDirection::DOWN;
            bool m_invalidated = false;

    };  
}

#endif