#ifndef __QANVAS_BOX_CONTAINER_HPP__
#define __QANVAS_BOX_CONTAINER_HPP__

#include <qanvas/widgets/container.hpp>

namespace Qore::Qanvas {
    enum class BoxAxis {
        HORIZONTAL,
        VERTICAL
    };

    class BoxContainer : public Container {
        public:
            explicit BoxContainer(BoxAxis axis, uint16_t separation = 4);

            void setSeparation(uint16_t px);
            uint16_t separation() const;
        protected:
            Size compute() const override;
            void arrange() override;
        private:
            BoxAxis m_axis;
            uint16_t m_separation;
    };

    class HBoxContainer : public BoxContainer {
        public:
            explicit HBoxContainer(uint16_t separation = 4) : BoxContainer(BoxAxis::HORIZONTAL, separation) {}
    };
    class VBoxContainer : public BoxContainer {
        public:
            explicit VBoxContainer(uint16_t separation = 4) : BoxContainer(BoxAxis::VERTICAL, separation) {}
    };

}

#endif