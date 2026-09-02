#ifndef __QANVAS_MARGIN_CONTAINER_HPP__
#define __QANVAS_MARGIN_CONTAINER_HPP__

// widgets
#include <qanvas/widgets/container.hpp>

namespace Qore::Qanvas {
    class MarginContainer : public Container {
        public:
            void setMargin(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
            void setMargin(uint16_t uniform);
        protected:
            Size compute() const override;
            void arrange() override;
        private:
            uint16_t m_left = 0, m_top = 0, m_right = 0, m_bottom = 0;
    };
}

#endif