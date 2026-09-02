#include <qanvas/widgets/overlay.hpp>

// core
#include <qanvas/core/extent.hpp>
#include <qanvas/core/canvas_usage.hpp>

// draw
#include <qanvas/draw/compositor.hpp>

// widgets
#include <qanvas/widgets/scroll_container.hpp>

using namespace Qore::Qanvas;

bool footprintsOverlap(PhysicalRect a, PhysicalRect b) {
    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}

Overlay::Overlay(Compositor* c, Device* d, PhysicalRect a, int z, ScrollSuspendPolicy s) : m_compositor(c), m_device(d), m_canvas(Extent{(uint16_t)a.w, (uint16_t)a.h}, CanvasUsage::RENDER_TARGET), m_view(&m_canvas, a.x, a.y, &m_transform), m_z(z), m_scrollPolicy(s) {
    setCanvas(&m_view);
    setRect(ContentRect{0, 0, a.w, a.h});
    setVisible(false);
    setEnabled(false);
}

Overlay::~Overlay() {
    if (m_showing) hide();
}

void Overlay::setScrollSuspendPolicy(ScrollSuspendPolicy policy) {
    m_scrollPolicy = policy;
}
ScrollSuspendPolicy Overlay::scrollSuspendPolicy() const {
    return m_scrollPolicy;
}

void Overlay::show() {
    if (m_showing) return;
    m_showing = true;

    setVisible(true);
    setEnabled(true);

    m_compositor->registerView(&m_view, m_z);
    m_canvas.markDirty(ContentRect{0, 0, (int16_t)m_canvas.width(), (int16_t)m_canvas.height()});

    m_suspended.clear();
    if (m_scrollPolicy != ScrollSuspendPolicy::NONE) {
        PhysicalRect myFootprint;
        bool haveFootprint = m_view.physicalFootprint(&myFootprint);

        for (ScrollContainer* sc : m_compositor->scrollContainers()) {
            bool shouldSuspend = (m_scrollPolicy == ScrollSuspendPolicy::SUSPEND_ALL);

            if (!shouldSuspend && m_scrollPolicy == ScrollSuspendPolicy::SUSPEND_INTERSECTING && haveFootprint) {
                PhysicalRect scFootprint;
                if (sc->view()->physicalFootprint(&scFootprint)) {
                    shouldSuspend = footprintsOverlap(myFootprint, scFootprint);
                }
            }

            if (shouldSuspend) {
                sc->suspend();
                m_suspended.push_back(sc);
            }
        }
    }
}

void Overlay::hide() {
    if (!m_showing) return;

    for (ScrollContainer* sc : m_suspended) sc->resume();
    m_suspended.clear();

    PhysicalRect footprint;
    if (m_view.physicalFootprint(&footprint)) {
        m_compositor->reclaim(footprint, &m_view);
    }

    m_compositor->unregisterView(&m_view);
    m_showing = false;

    setVisible(false);
    setEnabled(false);
}

bool Overlay::showing() const {
    return m_showing;
}
CanvasView* Overlay::view() {
    return &m_view;
}