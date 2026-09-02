#ifndef __QANVAS_COMMAND_HPP__
#define __QANVAS_COMMAND_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/content_rect.hpp>

// command
#include <qanvas/command/exec_context.hpp>

namespace Qore::Qanvas {
    class Command {
        public:
            virtual ~Command() = default;
            virtual void execute(ExecContext* context) = 0;
        private:
    };
}

#endif