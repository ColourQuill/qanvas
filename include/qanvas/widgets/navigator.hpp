#ifndef __QANVAS_NAVIGATOR_HPP__
#define __QANVAS_NAVIGATOR_HPP__

// std
#include <vector>

namespace Qore::Qanvas {
    class Widget;
    struct Event;

    class Navigator {
        public:
            void rebuild(Widget* root, Widget* preferredFocus = nullptr);

            void dispatch(const Event& e);

            Widget* current() const;
        private:
            void collect(Widget* w);
            void moveFocus(int direction);
            void setFocus(Widget* w, int direction = 1);

            std::vector<Widget*> m_stops;
            Widget* m_current = nullptr;
    };
}

#endif