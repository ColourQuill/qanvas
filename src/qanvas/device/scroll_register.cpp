#include <qanvas/device/scroll_register.hpp>

// std
#include <cstddef>
#include <iostream>

// core
#include <qanvas/core/physical_rect.hpp>

// device
#include <qanvas/device/transport.hpp>

using namespace Qore::Qanvas;

void ScrollRegister::configure(Transport* transport, uint16_t x, uint16_t top, uint16_t length) {
    m_transport = transport;
    m_x = x;
    m_top = top;
    m_length = length;
}

uint16_t ScrollRegister::x() const {
    return m_x;
}
uint16_t ScrollRegister::top() const {
    return m_top;
}
uint16_t ScrollRegister::length() const {
    return m_length;
}

uint16_t ScrollRegister::start(ScrollDirection dir) const {
    if (dir == ScrollDirection::DOWN) return m_top;
    return m_transport->height() - m_top - m_length;
}

void ScrollRegister::orient(ScrollDirection dir) {
    bool up = (dir == ScrollDirection::UP);
    m_transport->setMirrored(up);

    uint16_t panelFixedBottom = m_transport->height() - m_top - m_length;

    if (up) m_transport->defineScrollWindow(panelFixedBottom, m_top);
    else m_transport->defineScrollWindow(m_top, panelFixedBottom);
}

void ScrollRegister::reset(ScrollDirection dir) {
    m_line = 0;
    m_hardwareOffset = 0;
    m_direction = dir;
    orient(dir);
    m_transport->setScrollOffset(start(dir));
}

void ScrollRegister::advance(int64_t step, ScrollDirection dir) {
    if (dir != m_direction) {
        m_hardwareOffset = static_cast<uint16_t>(invert(m_hardwareOffset));
        int64_t shift = (dir == ScrollDirection::UP) ? -(int64_t)m_length : (int64_t)m_length;
        m_line = wrap(invert(wrap(m_line + shift)));
        m_direction = dir;
        orient(dir);
    }

    m_hardwareOffset = static_cast<uint16_t>(wrap(m_hardwareOffset + step));
    m_line += step;
    m_transport->setScrollOffset(start(dir) + m_hardwareOffset);
}
void ScrollRegister::flip(ScrollDirection dir) {
    if (m_direction == dir) return;

    m_direction = dir;
    m_hardwareOffset = (uint16_t)invert(m_hardwareOffset);
    int64_t h = (dir == ScrollDirection::UP) ? -(int64_t)m_length : (int64_t)m_length;
    m_line += h;
    m_line = wrap(invert(wrap(m_line)));

    orient(dir);
    m_transport->setScrollOffset(start(dir) + m_hardwareOffset);
}

ScrollDirection ScrollRegister::direction() const {
    return m_direction;
}
uint16_t ScrollRegister::hardwareOffset() const {
    return m_hardwareOffset;
}
int64_t ScrollRegister::wrappedAddress(int64_t offset) const {
    return wrap(m_line + offset);
}

void ScrollRegister::write(ScrollDirection dir, uint16_t wrappedY, uint16_t w, uint16_t h, const uint16_t* data) {
    uint16_t base = start(dir);

    if ((uint32_t)wrappedY + h <= m_length) {
        m_transport->writeRect(PhysicalRect{(int16_t)m_x, (int16_t)(base + wrappedY), (int16_t)w, (int16_t)h}, data);
        return;
    }
    uint16_t firstH = m_length - wrappedY;
    uint16_t secondH = h - firstH;
    m_transport->writeRect(PhysicalRect{(int16_t)m_x, (int16_t)(base + wrappedY), (int16_t)w, (int16_t)firstH}, data);
    m_transport->writeRect(PhysicalRect{(int16_t)m_x, (int16_t)base, (int16_t)w, (int16_t)secondH}, data + (size_t)firstH * w);
}
void ScrollRegister::write(ScrollDirection dir, uint16_t x, uint16_t wrappedY, uint16_t w, uint16_t h, const uint16_t* data) {
    uint16_t base = start(dir);
    if ((uint32_t)wrappedY + h <= m_length) {
        m_transport->writeRect(PhysicalRect{(int16_t)x, (int16_t)(base + wrappedY), (int16_t)w, (int16_t)h}, data);
        return;
    }
    uint16_t firstH = m_length - wrappedY;
    uint16_t secondH = h - firstH;
    m_transport->writeRect(PhysicalRect{(int16_t)x, (int16_t)(base + wrappedY), (int16_t)w, (int16_t)firstH}, data);
    m_transport->writeRect(PhysicalRect{(int16_t)x, (int16_t)base, (int16_t)w, (int16_t)secondH}, data + (size_t)firstH * w);
}

int64_t ScrollRegister::wrap(int64_t p) const {
    int64_t r = p % (int64_t)m_length;
    return r < 0 ? r + (int64_t)m_length : r;
}
int64_t ScrollRegister::invert(int64_t p) const {
    return (int64_t)m_length - p;
}