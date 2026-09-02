#include <qanvas/command/command_buffer.hpp>

// command
#include <qanvas/command/command.hpp>
#include <qanvas/command/blit.hpp>
#include <qanvas/command/scroll_sync.hpp>
#include <qanvas/command/mirror_barrier.hpp>

using namespace Qore::Qanvas;

CommandBuffer::~CommandBuffer() {
    clear();
}

void CommandBuffer::blit(CanvasView* view, ContentRect rect) {
    m_commands.push_back(new Blit(view, rect));
}

void CommandBuffer::scroll(CanvasView* view, int64_t offset) {
    m_commands.push_back(new ScrollSync(view, offset));
}

void CommandBuffer::mirrorBarrier(bool mirrored) {
    m_commands.push_back(new MirrorBarrier(mirrored));
}

void CommandBuffer::executeAll(ExecContext* ctx) {
    for (Command* c : m_commands) {
        c->execute(ctx);
    }
}

void CommandBuffer::clear() {
    for (Command* c : m_commands) {
        delete c;
    }
    m_commands.clear();
}