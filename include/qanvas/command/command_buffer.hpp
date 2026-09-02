#ifndef __QANVAS_COMMAND_BUFFER_HPP__
#define __QANVAS_COMMAND_BUFFER_HPP__

// std
#include <cstdint>
#include <vector>

// core
#include <qanvas/core/content_rect.hpp>

namespace Qore::Qanvas {
    class Command;
    class CanvasView;
    struct ExecContext;

    class CommandBuffer {
        public:
            ~CommandBuffer();

            void blit(CanvasView* view, ContentRect rect);
            void scroll(CanvasView* view, int64_t offset);
            void mirrorBarrier(bool mirrored);

            void executeAll(ExecContext* ctx);
            void clear();
        private:
            std::vector<Command*> m_commands;
    };
}

#endif