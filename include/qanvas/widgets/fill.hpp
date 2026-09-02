#ifndef __QANVAS_FILL_HPP__
#define __QANVAS_FILL_HPP__

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/background.hpp>
#include <qanvas/core/fit_mode.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Fill : public Widget {
        public:
            explicit Fill(Color color);
            explicit Fill(const Background& background);

            void setColor(Color color);
            Color color() const;

            void setImage(Image* image, FitMode fit = FitMode::COVER);
            void setBackground(const Background& background);
            const Background& background() const;

            void draw(Canvas* canvas, ContentRect canvasSpaceRect) override;
        private:
            Background m_background;
    };
}

#endif