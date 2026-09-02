#ifndef __QANVAS_SIZE_FLAG_HPP__
#define __QANVAS_SIZE_FLAG_HPP__

// std
#include <cstdint>

namespace Qore::Qanvas {
    enum class SizeFlag : uint8_t { 
        SHRINK_BEGIN = 0,
        FILL = 1 << 0,
        EXPAND = 1 << 1,
        SHRINK_CENTER = 1 << 2,
        SHRINK_END = 1 << 3
    };

    inline SizeFlag operator|(SizeFlag a, SizeFlag b) {
        return static_cast<SizeFlag>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }
    inline bool hasFlag(SizeFlag value, SizeFlag flag) {
        return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
    }
}

#endif