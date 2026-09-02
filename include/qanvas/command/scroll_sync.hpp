#ifndef __QANVAS_SCROLL_SYNC_HPP__
#define __QANVAS_SCROLL_SYNC_HPP__

// command
#include <qanvas/command/command.hpp>

namespace Qore::Qanvas {
    class CanvasView;

    class ScrollSync : public Command {
        public:
            ScrollSync(CanvasView* view, int64_t offset);
            void execute(ExecContext* ctx) override;
        private:
            CanvasView* m_view;
            int64_t m_offset;
    };
}

#endif