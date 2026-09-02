#ifndef __QANVAS_MIRROR_BARRIER_HPP__
#define __QANVAS_MIRROR_BARRIER_HPP__

// command
#include <qanvas/command/command.hpp>

namespace Qore::Qanvas {
    class MirrorBarrier : public Command {
        public:
            explicit MirrorBarrier(bool mirrored);
            void execute(ExecContext* ctx) override;
        private:
            bool m_mirrored;
    };
}

#endif