#include <qanvas/command/blit.hpp>

// device
#include <qanvas/device/device.hpp>

using namespace Qore::Qanvas;

Blit::Blit(CanvasView* view, ContentRect rect) : m_view(view), m_rect(rect) {}

void Blit::execute(ExecContext* context) {
    context->device->blit(m_view, m_rect);
}