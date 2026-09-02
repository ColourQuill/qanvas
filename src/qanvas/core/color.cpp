#include <qanvas/core/color.hpp>

using namespace Qore::Qanvas;

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

uint16_t Color::pack() const {
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
Color Color::unpack(uint16_t packed) {
    uint8_t r5 = (packed >> 11) & 0x1F;
    uint8_t g6 = (packed >> 5) & 0x3F;
    uint8_t b5 = packed & 0x1F;
    uint8_t r = (uint8_t)((r5 << 3) | (r5 >> 2));
    uint8_t g = (uint8_t)((g6 << 2) | (g6 >> 4));
    uint8_t b = (uint8_t)((b5 << 3) | (b5 >> 2));
    return Color(r, g, b, 255);
}

Color Color::blend(const Color& bg) const {
    if (a == 255) return *this;
    if (a == 0) return bg;

    uint16_t alpha = a;
    uint16_t invAlpha = 255 - a;

    Color out;
    out.r = static_cast<uint8_t>((r * alpha + bg.r * invAlpha + 127) / 255);
    out.g = static_cast<uint8_t>((g * alpha + bg.g * invAlpha + 127) / 255);
    out.b = static_cast<uint8_t>((b * alpha + bg.b * invAlpha + 127) / 255);
    out.a = static_cast<uint8_t>(alpha + (bg.a * invAlpha + 127) / 255);

    return out;
}

bool Qore::Qanvas::operator==(const Color& a, const Color& b) {
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a);
}

Color Qore::Qanvas::operator*(const Color& a, const Color& b) {
    return Color(
        (a.r * b.r) / 255,
        (a.g * b.g) / 255,
        (a.b * b.b) / 255,
        (a.a * b.a) / 255
    );
}

const Color Color::RED {255, 0, 0};
const Color Color::GREEN {0, 255, 0};
const Color Color::BLUE {0, 0, 255};
const Color Color::YELLOW {255, 255, 0};
const Color Color::CYAN {0, 255, 255};
const Color Color::MAGENTA {255, 0, 255};
const Color Color::WHITE {255, 255, 255};
const Color Color::BLACK {0, 0, 0};