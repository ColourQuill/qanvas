#ifndef __QANVAS_BLIT_HPP__
#define __QANVAS_BLIT_HPP__

// core
#include <qanvas/core/content_rect.hpp>

// command
#include <qanvas/command/command.hpp>

namespace Qore::Qanvas {
    class CanvasView;

    class Blit : public Command {
        public:
            Blit(CanvasView* view, ContentRect rect);
            void execute(ExecContext* ctx) override;
        private:
            CanvasView* m_view;
            ContentRect m_rect;
    };
}

#endif