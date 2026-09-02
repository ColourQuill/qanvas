#ifndef __QANVAS_PANEL_HPP__
#define __QANVAS_PANEL_HPP__

// core
#include <qanvas/core/identity_transform.hpp>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/canvas_view.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Compositor;

    class Panel : public Widget {
        public:
            Panel(Compositor* compositor, ContentRect rect, int16_t deviceOriginX, int16_t deviceOriginY);
            ~Panel() override;
        private:
            Compositor* m_compositor;
            Canvas m_canvas;
            IdentityTransform m_transform;
            CanvasView m_view;
    };
}

#endif