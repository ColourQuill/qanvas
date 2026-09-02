#include <qanvas/draw/compositor.hpp>

// std
#include <algorithm>

// core
#include <qanvas/core/coord_transform.hpp>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/canvas_view.hpp>

// command
#include <qanvas/command/command_buffer.hpp>

// device
#include <qanvas/device/device.hpp>

using namespace Qore::Qanvas;

Compositor::Compositor(Device* device) : m_device(device) {}

void Compositor::registerView(CanvasView* view, int z) {
    m_views.push_back({view, z});
    std::stable_sort(m_views.begin(), m_views.end(), [](const Entry& a, const Entry& b) { return a.z < b.z; });
}

void Compositor::unregisterView(CanvasView* view) {
    m_views.erase(std::remove_if(m_views.begin(), m_views.end(), [view](const Entry& e) { return e.view == view; }), m_views.end());
}

void Compositor::registerScrollContainer(ScrollContainer* sc) {
    m_scrollContainers.push_back(sc);
}
void Compositor::unregisterScrollContainer(ScrollContainer* sc) {
    m_scrollContainers.erase(std::remove(m_scrollContainers.begin(), m_scrollContainers.end(), sc), m_scrollContainers.end());
}
const std::vector<ScrollContainer*>& Compositor::scrollContainers() const {
    return m_scrollContainers;
}

void Compositor::reclaim(PhysicalRect area, CanvasView* above) {
    int aboveZ = 0;
    bool found = false;
    for (auto& e : m_views) {
        if (e.view == above) { aboveZ = e.z; found = true; break; }
    }
    if (!found) return;

    for (auto& e : m_views) {
        if (e.z >= aboveZ) continue;
        ContentRect out;
        if (e.view->physicalToContentClip(area, &out)) {
            e.view->canvas()->markDirty(out);
        }
    }
}

void Compositor::render() {
    if (m_views.empty()) return;

    CommandBuffer cmd;

    for (size_t i = 0; i < m_views.size(); ++i) {
        CanvasView* view = m_views[i].view;

        if (view->transform()->consumeInvalidated()) {
            Canvas* canvas = view->canvas();
            canvas->markDirty(ContentRect{0, 0, (int16_t)canvas->width(), (int16_t)canvas->height()});
        }

        ContentRect dirty;
        while (view->canvas()->takeDirty(dirty)) {
            cmd.blit(view, dirty);

            PhysicalRect phys[1];
            if (view->mapToPhysical(dirty, phys) == 0) continue;

            for (size_t j = i + 1; j < m_views.size(); ++j) {
                CanvasView* higher = m_views[j].view;
                ContentRect reclaimed;
                if (higher->physicalToContentClip(phys[0], &reclaimed)) {
                    higher->canvas()->markDirty(reclaimed);
                }
            }
        }
    }

    m_device->submit(&cmd);
}