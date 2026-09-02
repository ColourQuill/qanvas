#ifndef __QANVAS_KEYBOARD_HPP__
#define __QANVAS_KEYBOARD_HPP__

// std
#include <functional>

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/background.hpp>
#include <qanvas/core/fit_mode.hpp>

// widgets
#include <qanvas/widgets/overlay.hpp>

namespace Qore::Qanvas {
    class Navigator;
    class Compositor;

    class Keyboard : public Overlay {
        public:
            Keyboard(Compositor* compositor, Device* device, PhysicalRect area, Navigator* navigator, Widget* appRoot, int z = 1, ScrollSuspendPolicy scrollPolicy = ScrollSuspendPolicy::SUSPEND_INTERSECTING);

            void open(Widget* requester);
            void close();

            void setOnCharacter(std::function<void(char)> cb);
            void setOnBackspace(std::function<void()> cb);
            void setOnDone(std::function<void()> cb);

            void setBackgroundColor(Color color);
            void setBackgroundImage(Image* image, FitMode fit = FitMode::COVER);
            void setBackground(const Background& background);

            void draw(Canvas* canvas, ContentRect r) override;

            virtual void reset();
        protected:
            void emitCharacter(char c);
            void emitBackspace();
            void emitDone();
        private:
            Navigator* m_navigator;
            Widget* m_appRoot;
            Widget* m_requester = nullptr;

            std::function<void(char)> m_onCharacter;
            std::function<void()> m_onBackspace;
            std::function<void()> m_onDone;

            Background m_background{Color{25, 25, 27}};
    };
}

#endif