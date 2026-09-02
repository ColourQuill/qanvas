#ifndef __QANVAS_COLOR_HPP__
#define __QANVAS_COLOR_HPP__

// std
#include <cstdint>

namespace Qore::Qanvas {
    struct Color {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;

        Color() = default;
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        uint16_t pack() const;
        Color blend(const Color& bg) const;

        static Color unpack(uint16_t packed);

        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;
        static const Color WHITE;
        static const Color BLACK;
    };

    bool operator==(const Color& a, const Color& b);
    Color operator*(const Color& a, const Color& b);
}

#endif