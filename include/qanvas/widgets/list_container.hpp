#ifndef __QANVAS_LIST_CONTAINER_HPP__
#define __QANVAS_LIST_CONTAINER_HPP__

// widgets
#include <qanvas/widgets/box_container.hpp>

namespace Qore::Qanvas {
    class ListContainer : public BoxContainer {
        public:
            explicit ListContainer(BoxAxis axis, uint16_t separation = 4) : BoxContainer(axis, separation) {}
    };

    class HListContainer : public ListContainer {
        public:
            explicit HListContainer(uint16_t separation = 4) : ListContainer(BoxAxis::HORIZONTAL, separation) {}
    };
    class VListContainer : public ListContainer {
        public:
            explicit VListContainer(uint16_t separation = 4) : ListContainer(BoxAxis::VERTICAL, separation) {}
    };
}

#endif