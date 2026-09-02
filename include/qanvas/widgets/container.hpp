#ifndef __QANVAS_CONTAINER_HPP__
#define __QANVAS_CONTAINER_HPP__

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Container : public Widget {
        public:
            void setRect(ContentRect r) override;
            void queueSort() override;
            Size minimumSize() const override final;
        protected:
            virtual Size compute() const = 0;
            virtual void arrange() = 0;
    };
}

#endif