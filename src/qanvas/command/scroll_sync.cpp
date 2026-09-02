#include <qanvas/command/scroll_sync.hpp>

// device
#include <qanvas/device/device.hpp>

using namespace Qore::Qanvas;

ScrollSync::ScrollSync(CanvasView* view, int64_t offset) : m_view(view), m_offset(offset) {}

void ScrollSync::execute(ExecContext* context) {
    context->device->scroll(m_view, m_offset);
}