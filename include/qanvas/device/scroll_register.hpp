#ifndef __QANVAS_SCROLL_REGISTER_HPP__
#define __QANVAS_SCROLL_REGISTER_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/scroll_direction.hpp>

namespace Qore::Qanvas {
    class Transport;

    class ScrollRegister {
        public:
            void configure(Transport* transport, uint16_t x, uint16_t top, uint16_t length);

            uint16_t x() const;
            uint16_t top() const;
            uint16_t length() const;

            uint16_t start(ScrollDirection dir) const;

            void orient(ScrollDirection dir);
            void reset(ScrollDirection dir);
            void advance(int64_t step, ScrollDirection dir);
            void flip(ScrollDirection dir);

            ScrollDirection direction() const;
            uint16_t hardwareOffset() const;
            int64_t wrappedAddress(int64_t offset = 0) const;

            void write(ScrollDirection dir, uint16_t x, uint16_t wrappedY, uint16_t w, uint16_t h, const uint16_t* data);
            void write(ScrollDirection dir, uint16_t wrappedY, uint16_t w, uint16_t h, const uint16_t* data);
        private:
            int64_t wrap(int64_t p) const;
            int64_t invert(int64_t p) const;

            Transport* m_transport = nullptr;
            uint16_t m_x = 0;
            uint16_t m_top = 0;
            uint16_t m_length = 0;

            int64_t m_line = 0;
            uint16_t m_hardwareOffset = 0;
            ScrollDirection m_direction = ScrollDirection::DOWN;
    };
}

#endif