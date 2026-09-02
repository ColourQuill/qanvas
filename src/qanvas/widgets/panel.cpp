#include <qanvas/widgets/panel.hpp>

// draw
#include <qanvas/draw/compositor.hpp>

using namespace Qore::Qanvas;

Panel::Panel(Compositor* compositor, ContentRect rect, int16_t deviceOriginX, int16_t deviceOriginY) : 
    m_compositor(compositor),
    m_canvas(Extent{(uint16_t)rect.w, (uint16_t)rect.h}, CanvasUsage::RENDER_TARGET),
    m_transform(),
    m_view(&m_canvas, deviceOriginX, deviceOriginY, &m_transform)
{
    setRect(rect);
    setCanvas(&m_view);
    m_compositor->registerView(&m_view);
}

Panel::~Panel() {
    m_compositor->unregisterView(&m_view);
}