#ifndef __QANVAS_OVERLAY_HPP__
#define __QANVAS_OVERLAY_HPP__

// std
#include <vector>

// core
#include <qanvas/core/physical_rect.hpp>
#include <qanvas/core/identity_transform.hpp>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/canvas_view.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Compositor;
    class Device;
    class ScrollContainer;

    enum class ScrollSuspendPolicy {
        NONE,
        SUSPEND_ALL,
        SUSPEND_INTERSECTING
    };

    class Overlay : public Widget {
        public:
            Overlay(Compositor* compositor, Device* device, PhysicalRect area, int z = 1, ScrollSuspendPolicy scrollPolicy = ScrollSuspendPolicy::SUSPEND_INTERSECTING);
            ~Overlay() override;

            void setScrollSuspendPolicy(ScrollSuspendPolicy policy);
            ScrollSuspendPolicy scrollSuspendPolicy() const;

            void show();
            void hide();
            bool showing() const;

            CanvasView* view();
        private:
            Compositor* m_compositor;
            Device* m_device;
            Canvas m_canvas;
            IdentityTransform m_transform;
            CanvasView m_view;
            int m_z;
            ScrollSuspendPolicy m_scrollPolicy;
            bool m_showing = false;
            std::vector<ScrollContainer*> m_suspended;
    };
}

#endif