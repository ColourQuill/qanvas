#include <qanvas/widgets/container.hpp>

// std
#include <algorithm>

using namespace Qore::Qanvas;

void Container::setRect(ContentRect r) {
    Widget::setRect(r);
    arrange();
}
void Container::queueSort() {
    arrange();
    Widget::queueSort();
}
Size Container::minimumSize() const {
    Size fromChildren = compute();
    return { std::max(fromChildren.w, m_minSize.w), std::max(fromChildren.h, m_minSize.h) };
}