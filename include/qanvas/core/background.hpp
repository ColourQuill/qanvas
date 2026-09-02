#ifndef __QANVAS_BACKGROUND_HPP__
#define __QANVAS_BACKGROUND_HPP__

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/fit_mode.hpp>

namespace Qore::Qanvas {
    class Image;

    class Background {
        public:
            Background() = default;
            Background(Color color);
            Background(Image* image, FitMode fit = FitMode::COVER, Color color = Color::WHITE);

            void setColor(Color c);
            Color color() const;

            void setImage(Image* image);
            Image* image() const;

            void setFit(FitMode fit);
            FitMode fit() const;
        private:
            Color m_color = Color::WHITE;
            Image* m_image = nullptr;
            FitMode m_fit = FitMode::COVER;
    };
}

#endif