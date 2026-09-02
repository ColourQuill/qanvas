#ifndef __QANVAS_CANVAS_USAGE_HPP__
#define __QANVAS_CANVAS_USAGE_HPP__

// std
#include <cstdint>

namespace Qore::Qanvas {
    enum class CanvasUsage : uint8_t {
        NONE = 0,
        RENDER_TARGET = 1 << 0,
        SCROLL_SOURCE = 1 << 1,
    };

    constexpr CanvasUsage operator|(CanvasUsage a, CanvasUsage b) {
        return static_cast<CanvasUsage>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }
    constexpr CanvasUsage operator&(CanvasUsage a, CanvasUsage b) {
        return static_cast<CanvasUsage>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }
    constexpr bool hasUsage(CanvasUsage value, CanvasUsage flag) {
        return (value & flag) == flag;
    }
};

#endif