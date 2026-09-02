#ifndef __QANVAS_BUTTON_HPP__
#define __QANVAS_BUTTON_HPP__

// std
#include <functional>

// widgets
#include <qanvas/widgets/container.hpp>

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/background.hpp>
#include <qanvas/core/fit_mode.hpp>

namespace Qore::Qanvas {
    enum class ButtonState {
        IDLE,
        SELECTED,
        ACTIVATED,
        DISABLED
    };
    
    class Button : public Container {
        public:
            Button();

            void draw(Canvas* canvas, ContentRect canvasSpaceRect) override;
            bool handle(const Event& e) override;
            bool isFocusStop() const override;

            void setIdleColor(Color c);
            void setIdleImage(Image* image, FitMode fit = FitMode::COVER);
            void setIdleBackground(const Background& bg);

            void setSelectedColor(Color c);
            void setSelectedImage(Image* image, FitMode fit = FitMode::COVER);
            void setSelectedBackground(const Background& bg);

            void setActivatedColor(Color c);
            void setActivatedImage(Image* image, FitMode fit = FitMode::COVER);
            void setActivatedBackground(const Background& bg);

            void setDisabledColor(Color c);
            void setDisabledImage(Image* image, FitMode fit = FitMode::COVER);
            void setDisabledBackground(const Background& bg);

            void setCallback(std::function<void()> cb);
        protected:
            Size compute() const override;
            void arrange() override;
        private:
            Widget* m_content = nullptr;
            std::function<void()> m_onActivated;
            ButtonState m_state = ButtonState::IDLE;

            // Defaults arbitrary, may change.
            Background m_idleBackground{Color{45, 45, 48}};
            Background m_selectedBackground{Color{0, 122, 204}};
            Background m_activatedBackground{Color{30, 160, 255}};
            Background m_disabledBackground{Color{35, 35, 37}};

            bool m_arranging = false;
    };
}

#endif