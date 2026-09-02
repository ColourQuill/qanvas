#include <qanvas/widgets/scroll_container.hpp>

// std
#include <algorithm>
#include <iostream>

// core
#include <qanvas/core/scroll_direction.hpp>

// events
#include <qanvas/events/widget_event.hpp>
#include <qanvas/events/input_event.hpp>

// device
#include <qanvas/device/device.hpp>

// draw
#include <qanvas/draw/compositor.hpp>

using namespace Qore::Qanvas;

ScrollContainer::ScrollContainer(Widget* parent, Compositor* compositor, Device* device, ContentRect rect, uint16_t contentHeight)
    : m_compositor(compositor),
      m_device(device),
      m_canvas(Extent{(uint16_t)rect.w, contentHeight}, CanvasUsage::RENDER_TARGET | CanvasUsage::SCROLL_SOURCE),
      m_transform(contentHeight, (uint16_t)rect.h),
      m_view(&m_canvas, Widget::resolveDeviceOriginX(parent, rect), Widget::resolveDeviceOriginY(parent, rect), &m_transform)
{
    setRect(rect);
    setCanvas(&m_view);
    parent->add(this);
    m_compositor->registerView(&m_view);
    m_compositor->registerScrollContainer(this);

    ContentRect nestedRect;
    m_nestedParentView = Widget::resolveAncestorCanvasView(parent, rect, &nestedRect);
    if (m_nestedParentView) {
        m_nestedParentView->registerNestedView(this, nestedRect, &m_view);
        m_view.setNestedParent(m_nestedParentView, nestedRect);
        m_nestedRect = nestedRect;
    }
}

ScrollContainer::~ScrollContainer() {
    if (m_nestedParentView) m_nestedParentView->unregisterNestedView(this);
    if (m_active) m_device->deactivateScroll();
    m_compositor->unregisterScrollContainer(this);
    m_compositor->unregisterView(&m_view);
}

void ScrollContainer::setActivationMode(ActivationMode mode) {
    m_activationMode = mode;
}
ActivationMode ScrollContainer::activationMode() const {
    return m_activationMode;
}

void ScrollContainer::setSteps(int64_t steps) {
    m_steps = steps;
}
int64_t ScrollContainer::steps() const {
    return m_steps;
}

void ScrollContainer::setResistance(int threshold) {
    m_resistanceThreshold = threshold;
}
int ScrollContainer::resistance() const {
    return m_resistanceThreshold;
}

void ScrollContainer::activate(int64_t startPosition) {
    if (m_suspendedForChild) return;
    int64_t resumeAt = (startPosition >= 0) ? startPosition : m_transform.offset();
    m_device->activateScroll(&m_view, resumeAt, ScrollDirection::DOWN);
    m_active = true;
    redrawSides();
}

void ScrollContainer::deactivate() {
    if (!m_active) return;
    m_device->deactivateScroll();
    m_active = false;
    // redrawSides();
}

bool ScrollContainer::active() const {
    return m_active;
}

void ScrollContainer::scrollBy(int64_t delta) {
    if (m_suspendedForChild) return;
    m_device->scroll(&m_view, m_transform.offset() + delta);
    redrawSides();
}

void ScrollContainer::scrollTo(int64_t position) {
    if (m_suspendedForChild) return;
    m_device->scroll(&m_view, position);
    redrawSides();
}

bool ScrollContainer::isFocusStop() const {
    return true;
}

void ScrollContainer::refreshFocusable() {
    m_focusable.clear();
    for (Widget* c : children()) {
        Widget::collectFocusStops(c, &m_focusable);
    }
}

void ScrollContainer::deselectCurrent() {
    if (m_selectedIndex < 0 || m_selectedIndex >= (int)m_focusable.size()) return;
    m_focusable[m_selectedIndex]->handle(WidgetEvent(WidgetAction::FOCUS_LOST));
}

void ScrollContainer::selectChild(int index) {
    deselectCurrent();
    m_selectedIndex = index;
    resetEdgeResistance();
 
    Widget* child = m_focusable[index];
    int64_t viewStart = m_transform.offset();
    int64_t viewEnd = viewStart + m_transform.viewportHeight();
    int64_t childTop = child->rect().y;
    int64_t childBottom = childTop + child->rect().h;
 
    if (childTop < viewStart) scrollTo(childTop);
    else if (childBottom > viewEnd) scrollTo(childBottom - m_transform.viewportHeight());
 
    child->handle(WidgetEvent(WidgetAction::FOCUS_GAINED));
}

bool ScrollContainer::selectChild(Widget* target) {
    refreshFocusable();
    auto it = std::find(m_focusable.begin(), m_focusable.end(), target);
    if (it == m_focusable.end()) return false;
    m_navMode = NavigationMode::SNAP;
    selectChild((int)std::distance(m_focusable.begin(), it));
    return true;
}

void ScrollContainer::suspend() {
    if (m_suspendedForChild) return;
    m_suspendedForChild = true;
    m_wasActiveBeforeSuspend = m_active;
    if (m_active) deactivate();
}

void ScrollContainer::resume() {
    if (!m_suspendedForChild) return;
    m_suspendedForChild = false;
    if (m_wasActiveBeforeSuspend) activate();
}

bool ScrollContainer::suspended() const {
    return m_suspendedForChild;
}

CanvasView* ScrollContainer::view() {
    return &m_view;
}

bool ScrollContainer::overcomesEdgeResistance(int direction) {
    if (direction != m_lastEdgeDirection) {
        m_edgeResistanceCounter = 0;
        m_lastEdgeDirection = direction;
    }
 
    m_edgeResistanceCounter++;
    if (m_edgeResistanceCounter < m_resistanceThreshold) {
        return false;
    }
 
    resetEdgeResistance();
    return true;
}

void ScrollContainer::resetEdgeResistance() {
    m_edgeResistanceCounter = 0;
    m_lastEdgeDirection = 0;
}

bool ScrollContainer::childEdgeApproaching(const Widget* child, int direction) const {
    int64_t viewStart = m_transform.offset();
    int64_t viewEnd = viewStart + m_transform.viewportHeight();
 
    if (direction > 0) {
        int64_t maxOffset = m_transform.maxOffset();
        int64_t top = child->rect().y;
        int64_t proposedEnd = std::min(viewEnd + m_steps, maxOffset + m_transform.viewportHeight());
 
        bool alreadyVisible = (top >= viewStart && top < viewEnd);
        bool aboutToCross = (top >= viewEnd && top < proposedEnd);
        return alreadyVisible || aboutToCross;
    } else {
        int64_t bottom = child->rect().y + child->rect().h;
        int64_t proposedStart = std::max(viewStart - m_steps, (int64_t)0);
 
        bool alreadyVisible = (bottom > viewStart && bottom <= viewEnd);
        bool aboutToCross = (bottom <= viewStart && bottom > proposedStart);
        return alreadyVisible || aboutToCross;
    }
}

int ScrollContainer::nearestVisibleChild(int direction) const {
    int64_t viewStart = m_transform.offset();
    int64_t viewEnd = viewStart + m_transform.viewportHeight();
 
    int best = -1;
    int64_t bestDistance = 0;
 
    for (int i = 0; i < (int)m_focusable.size(); ++i) {
        const Widget* child = m_focusable[i];
        int64_t top = child->rect().y;
        int64_t bottom = top + child->rect().h;
        bool visible = (bottom > viewStart && top < viewEnd);
        if (!visible) continue;
        int64_t distance = (direction > 0) ? (viewEnd - bottom) : (top - viewStart);
        if (best < 0 || distance < bestDistance) {
            best = i;
            bestDistance = distance;
        }
    }
 
    return best;
}


bool ScrollContainer::advance(int direction) {
    int64_t viewStart = m_transform.offset();
 
    if (direction > 0) {
        if (viewStart >= m_transform.maxOffset()) return false;
        scrollBy(m_steps);
    } else {
        if (viewStart <= 0) return false;
        scrollBy(-m_steps);
    }
    return true;
}

bool ScrollContainer::stepFree(int direction) {
    int64_t viewStart = m_transform.offset();
    int64_t maxOffset = m_transform.maxOffset();
 
    bool atTopMovingDown = (direction > 0 && viewStart <= 0);
    bool atBottomMovingUp = (direction < 0 && viewStart >= maxOffset);
    if (atTopMovingDown || atBottomMovingUp) {
        int nearest = nearestVisibleChild(-direction);
        if (nearest >= 0) {
            resetEdgeResistance();
            m_navMode = NavigationMode::SNAP;
            selectChild(nearest);
            return true;
        }
    }
 
    int candidateIndex = m_selectedIndex + direction;
    bool candidateExists = (direction > 0) ? candidateIndex < (int)m_focusable.size() : candidateIndex >= 0;
 
    if (candidateExists && childEdgeApproaching(m_focusable[candidateIndex], direction)) {
        if (!overcomesEdgeResistance(direction)) {
            advance(direction);
            return true;
        }
        m_navMode = NavigationMode::SNAP;
        selectChild(candidateIndex);
        return true;
    }
 
    resetEdgeResistance();
    advance(direction);
    return true;
}

bool ScrollContainer::stepSnap(int direction) {
    int next = m_selectedIndex + direction;
 
    if (next >= 0 && next < (int)m_focusable.size()) {
        selectChild(next);
        return true;
    }
 
    if (!overcomesEdgeResistance(direction)) {
        return true;
    }
 
    deselectCurrent();
    m_navMode = NavigationMode::FREE;
    m_selectedIndex = next;
    return stepFree(direction);
}

bool ScrollContainer::handle(const Event& e) {
    if (e.type() == EventType::WIDGET) {
        const auto& we = static_cast<const WidgetEvent&>(e);
        if (we.action != WidgetAction::FOCUS_LOST) return false;

        deselectCurrent();
        m_navMode = NavigationMode::FREE;
        m_selectedIndex = -1;
        resetEdgeResistance();
        if (m_activationMode != ActivationMode::MANUAL && m_active) deactivate();
        return true;
    }

    if (e.type() != EventType::INPUT) return false;
    const auto& ie = static_cast<const InputEvent&>(e);

    switch (ie.action) {
        case InputAction::BUTTON_PRESS:
            if (m_activationMode == ActivationMode::ON_PRESS && !m_active) { activate(); return true; }
            if (m_navMode == NavigationMode::SNAP && m_selectedIndex >= 0) {
                return m_focusable[m_selectedIndex]->handle(e);
            }
            return false;
        case InputAction::BUTTON_RELEASE:
            if (m_navMode == NavigationMode::SNAP && m_selectedIndex >= 0) {
                return m_focusable[m_selectedIndex]->handle(e);
            }
            return false;
        case InputAction::ROTATE_CLOCKWISE:
        case InputAction::ROTATE_ANTICLOCKWISE: {
            int direction = (ie.action == InputAction::ROTATE_CLOCKWISE) ? 1 : -1;
            if (m_navMode == NavigationMode::SNAP && m_selectedIndex >= 0) {
                if (m_focusable[m_selectedIndex]->handle(ie)) return true;
            }
            return (m_navMode == NavigationMode::SNAP) ? stepSnap(direction) : stepFree(direction);
        }
        default:
            return false;
    }
}

void ScrollContainer::onEnter(int direction) {
    refreshFocusable();
    m_navMode = NavigationMode::FREE;
    m_selectedIndex = -1;
    resetEdgeResistance();
 
    if (m_activationMode == ActivationMode::ON_FOCUS && !m_active) {
        activate();
    }
 
    if (direction < 0 && !m_focusable.empty()) {
        m_navMode = NavigationMode::SNAP;
        selectChild((int)m_focusable.size() - 1);
    }
}

void ScrollContainer::redrawSides() {
    if (!m_active || !m_nestedParentView) return;

    Canvas* parentCanvas = m_nestedParentView->canvas();
    uint16_t parentWidth = parentCanvas->width();

    int16_t leftX = 0;
    uint16_t leftW = (uint16_t)std::max<int16_t>(0, m_nestedRect.x);

    int16_t rightX = (int16_t)(m_nestedRect.x + m_nestedRect.w);
    uint16_t rightW = (rightX < (int16_t)parentWidth) ? (uint16_t)(parentWidth - rightX) : 0;

    if (leftW == 0 && rightW == 0) return;

    int16_t rowStart = m_nestedRect.y;
    uint16_t rows = (uint16_t)m_nestedRect.h;
    if (rowStart < 0 || (int32_t)rowStart + rows > parentCanvas->height()) return;

    int16_t deviceOriginX = m_nestedParentView->deviceOriginX();

    if (leftW > 0) {
        std::vector<uint16_t> strip((size_t)leftW * rows);
        for (uint16_t i = 0; i < rows; ++i) {
            const uint16_t* row = parentCanvas->row((uint16_t)(rowStart + i)) + leftX;
            std::copy(row, row + leftW, strip.data() + (size_t)i * leftW);
        }
        m_device->redrawSideStrip((uint16_t)(deviceOriginX + leftX), leftW, strip.data());
    }

    if (rightW > 0) {
        std::vector<uint16_t> strip((size_t)rightW * rows);
        for (uint16_t i = 0; i < rows; ++i) {
            const uint16_t* row = parentCanvas->row((uint16_t)(rowStart + i)) + rightX;
            std::copy(row, row + rightW, strip.data() + (size_t)i * rightW);
        }
        m_device->redrawSideStrip((uint16_t)(deviceOriginX + rightX), rightW, strip.data());
    }
}