#ifndef __QANVAS_SLIDER_HPP__
#define __QANVAS_SLIDER_HPP__

// std
#include <functional>

// core
#include <qanvas/core/color.hpp>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Slider : public Widget {
        public:
            Slider(int64_t minValue, int64_t maxValue, int64_t step = 1);

            void setValue(int64_t v);
            int64_t value() const;
            void setOnChanged(std::function<void(int64_t)> cb);

            void setTrackColor(Color c);
            void setFillColor(Color c);
            void setHandleColor(Color c);
            void setActiveHandleColor(Color c);

            bool isFocusStop() const override;
            bool handle(const Event& e) override;
            void draw(Canvas* canvas, ContentRect canvasSpaceRect) override;
        private:
            int64_t m_min, m_max, m_step;
            int64_t m_value;
            bool m_focused = false;
            bool m_active = false;
            std::function<void(int64_t)> m_onChanged;

            Color m_trackColor{60, 60, 64};
            Color m_fillColor{0, 122, 204};
            Color m_handleColor{255, 255, 255};
            Color m_activeHandleColor{255, 210, 90};

            void adjust(int64_t delta);
    };
}

#endif