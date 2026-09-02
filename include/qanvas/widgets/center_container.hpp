#ifndef __QANVAS_CENTER_CONTAINER_HPP__
#define __QANVAS_CENTER_CONTAINER_HPP__

// widgets
#include <qanvas/widgets/container.hpp>

namespace Qore::Qanvas {
    class CenterContainer : public Container {
        protected:
            Size compute() const override;
            void arrange() override;
    };
}

#endif