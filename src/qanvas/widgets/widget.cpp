#include <qanvas/widgets/widget.hpp>

// std
#include <algorithm>
#include <iostream>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/canvas_view.hpp>

using namespace Qore::Qanvas;

Widget::~Widget() {
    for (Widget* c : m_children) delete c;
    m_children.clear();
}

void Widget::add(Widget* child) {
    if (!child) return;
    child->m_parent = this;
    m_children.push_back(child);
    child->requestRedraw();
    queueSort();
}

void Widget::remove(Widget* child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it == m_children.end()) return;

    child->requestRedraw();
    m_children.erase(it);
    child->m_parent = nullptr;
    queueSort();
}

void Widget::clear() {
    for (Widget* c : m_children) {
        c->requestRedraw();
        delete c;
    }
    m_children.clear();
    queueSort();
}

bool Widget::contains(Widget* w) const {
    if (!w) return false;
    if (this == w) return true;
    for (Widget* c : m_children) {
        if (c->contains(w)) return true;
    }
    return false;
}

Widget* Widget::parent() const {
    return m_parent;
}
const std::vector<Widget*>& Widget::children() const {
    return m_children;
}

Size Widget::minimumSize() const {
    return m_minSize;
}
void Widget::setMinimumSize(Size s) {
    m_minSize = s;
    queueSort();
}

void Widget::setHSizeFlags(SizeFlag flags) {
    m_hFlags = flags;
    queueSort();
}
void Widget::setVSizeFlags(SizeFlag flags) {
    m_vFlags = flags;
    queueSort();
}
SizeFlag Widget::hSizeFlags() const {
    return m_hFlags;
}
SizeFlag Widget::vSizeFlags() const {
    return m_vFlags;
}

void Widget::setRect(ContentRect r) {
    if (r.x == m_rect.x && r.y == m_rect.y && r.w == m_rect.w && r.h == m_rect.h) return;
    requestRedraw();
    m_rect = r;
    requestRedraw();
}
ContentRect Widget::rect() const {
    return m_rect;
}

void Widget::queueSort() {
    if (m_parent) m_parent->queueSort();
}

void Widget::requestRedraw() {
    m_needsRedraw = true;
    markDirty();
}

void Widget::markDirty() {
    ContentRect accumulated = m_rect;
    Widget* current = this;
    while (current) {
        if (current->hasCanvas()) {
            current->canvas()->canvas()->markDirty(accumulated);
            return;
        }
        Widget* next = current->m_parent;
        if (!next) return;
        if (next->hasCanvas()) {
            next->canvas()->canvas()->markDirty(accumulated);
            return;
        }
        accumulated.x += next->m_rect.x;
        accumulated.y += next->m_rect.y;
        current = next;
    }
}

void Widget::drawAll(Canvas* canvas, ContentRect canvasSpaceRect) {
    if (!m_visible) return;

    Canvas* target = hasCanvas() ? m_ownedCanvas->canvas() : canvas;

    if (hasCanvas()) {
        target->clearDirtyRegions();
    }

    if (m_needsRedraw || target->intersectsDirty(canvasSpaceRect)) {
        draw(target, canvasSpaceRect);
        m_needsRedraw = false;
    }

    if (hasCanvas()) {
        for (Widget* c : m_children) c->drawAll(target, c->rect());
        return;
    }

    for (Widget* c : m_children) {
        ContentRect childRect{
            (int16_t)(canvasSpaceRect.x + c->rect().x),
            (int16_t)(canvasSpaceRect.y + c->rect().y),
            c->rect().w, c->rect().h
        };
        c->drawAll(canvas, childRect);
    }
}

void Widget::draw(Canvas*, ContentRect) {}

bool Widget::handle(const Event&) {
    return false;
}

void Widget::updateAll(uint32_t deltaMS) {
    update(deltaMS);
    for (Widget* c : children()) updateAll(deltaMS);
}


bool Widget::update(uint32_t) {
    return false;
}

void Widget::setVisible(bool v) {
    if (m_visible == v) return;
    m_visible = v;
    requestRedraw();
    queueSort();
}
bool Widget::visible() const {
    return m_visible;
}

void Widget::setEnabled(bool e) {
    if (m_enabled == e) return;
    m_enabled = e;
    requestRedraw();
    queueSort();
}
bool Widget::enabled() const {
    return m_enabled;
}

CanvasView* Widget::canvas() const {
    return m_ownedCanvas;
}
bool Widget::hasCanvas() const {
    return m_ownedCanvas != nullptr;
}
void Widget::setCanvas(CanvasView* view) {
    m_ownedCanvas = view;
}
void Widget::setClearColor(Color c) {
    if (hasCanvas()) m_ownedCanvas->canvas()->setClearColor(c);
}

bool Widget::isFocusStop() const {
    return false;
}
void Widget::onEnter(int) {}

int16_t Widget::resolveDeviceOriginX(Widget* parent, ContentRect ownRect) {
    ContentRect accumulated = ownRect;
    Widget* current = parent;
    while (current) {
        if (current->hasCanvas()) return (int16_t)(current->canvas()->deviceOriginX() + accumulated.x);
        accumulated.x += current->rect().x;
        current = current->parent();
    }
    return accumulated.x;
}
int16_t Widget::resolveDeviceOriginY(Widget* parent, ContentRect ownRect) {
    ContentRect accumulated = ownRect;
    Widget* current = parent;
    while (current) {
        if (current->hasCanvas()) return (int16_t)(current->canvas()->deviceOriginY() + accumulated.y);
        accumulated.y += current->rect().y;
        current = current->parent();
    }
    return accumulated.y;
}

CanvasView* Widget::resolveAncestorCanvasView(Widget* parent, ContentRect ownRect, ContentRect* outRect) {
    ContentRect accumulated = ownRect;
    Widget* current = parent;
    while (current) {
        if (current->hasCanvas()) {
            *outRect = accumulated;
            return current->canvas();
        }
        accumulated.x += current->rect().x;
        accumulated.y += current->rect().y;
        current = current->parent();
    }
    return nullptr;
}
void Widget::collectFocusStops(Widget* root, std::vector<Widget*>* out) {
    if (!root) return;
    if (!root->enabled()) return;
    if (root->isFocusStop()) { out->push_back(root); return; }
    for (Widget* c : root->children()) collectFocusStops(c, out);
}