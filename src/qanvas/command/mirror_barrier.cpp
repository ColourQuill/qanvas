#include <qanvas/command/mirror_barrier.hpp>

// device
#include <qanvas/device/device.hpp>

using namespace Qore::Qanvas;

MirrorBarrier::MirrorBarrier(bool mirrored) : m_mirrored(mirrored) {}

void MirrorBarrier::execute(ExecContext* context) {
    context->device->mirror(m_mirrored);
}