#ifndef __QANVAS_SCROLL_CONTAINER_HPP__
#define __QANVAS_SCROLL_CONTAINER_HPP__

// core
#include <qanvas/core/scroll_transform.hpp>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/canvas_view.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Compositor;
    class Device;

    enum class ActivationMode {
        MANUAL,
        ON_FOCUS,
        ON_PRESS
    };

    enum class NavigationMode {
        FREE,
        SNAP
    };

    class ScrollContainer : public Widget {
        public:
            ScrollContainer(Widget* parent, Compositor* compositor, Device* device, ContentRect rect, uint16_t contentHeight);
            ~ScrollContainer() override;

            void setActivationMode(ActivationMode mode);
            ActivationMode activationMode() const;

            void setSteps(int64_t steps);
            int64_t steps() const;

            void setResistance(int threshold);
            int resistance() const;

            void activate(int64_t startPosition = -1);
            void deactivate();
            bool active() const;

            void scrollBy(int64_t delta);
            void scrollTo(int64_t position);

            bool isFocusStop() const override;
            bool handle(const Event& e) override;
            void onEnter(int direction) override;

            bool selectChild(Widget* target);

            void suspend();
            void resume();
            bool suspended() const;
            CanvasView* view();
        private:
            void refreshFocusable();
            void deselectCurrent();
            void selectChild(int index);
            
            bool overcomesEdgeResistance(int direction);
            void resetEdgeResistance();
            bool childEdgeApproaching(const Widget* child, int direction) const;
            int nearestVisibleChild(int direction) const;

            bool advance(int direction);
            bool stepFree(int direction);
            bool stepSnap(int direction);

            void redrawSides();

            Compositor* m_compositor;
            Device* m_device;

            Canvas m_canvas;
            ScrollTransform m_transform;
            CanvasView m_view;
            CanvasView* m_nestedParentView = nullptr;
            ContentRect m_nestedRect{0, 0, 0, 0};

            ActivationMode m_activationMode = ActivationMode::ON_PRESS;
            bool m_active = false;
            bool m_suspendedForChild = false;
            bool m_wasActiveBeforeSuspend = false;

            std::vector<Widget*> m_focusable;
            NavigationMode m_navMode = NavigationMode::FREE;
            int m_selectedIndex = -1;
            int64_t m_steps = 20;
            
            int m_edgeResistanceCounter = 0;
            int m_lastEdgeDirection = 0;
            int m_resistanceThreshold = 4;
    };
}

#endif