#ifndef __QANVAS_COMPOSITOR_HPP__
#define __QANVAS_COMPOSITOR_HPP__

// std
#include <vector>

// core
#include <qanvas/core/physical_rect.hpp>

namespace Qore::Qanvas {
    class Device;
    class CanvasView;
    class ScrollContainer;

    class Compositor {
        public:
            explicit Compositor(Device* device);

            void registerView(CanvasView* view, int z = 0);
            void unregisterView(CanvasView* view);
            
            void reclaim(PhysicalRect area, CanvasView* above);

            void registerScrollContainer(ScrollContainer* sc);
            void unregisterScrollContainer(ScrollContainer* sc);
            const std::vector<ScrollContainer*>& scrollContainers() const;

            void render();
        private:
            struct Entry {
                CanvasView* view;
                int z;
            };
            Device* m_device;
            std::vector<Entry> m_views;
            std::vector<ScrollContainer*> m_scrollContainers;
    };
}

#endif