#ifndef __QANVAS_TRANSPORT_HPP__
#define __QANVAS_TRANSPORT_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/physical_rect.hpp>

namespace Qore::Qanvas {
    class Transport {
        public:
            virtual ~Transport() = default;

            virtual uint16_t height() const = 0;

            virtual void writeRect(PhysicalRect rect, const uint16_t* pixels) = 0;

            virtual void defineScrollWindow(uint16_t topFixed, uint16_t bottomFixed) = 0;
            virtual void setScrollOffset(uint16_t line) = 0;

            virtual void setMirrored(bool mirrored) = 0;

            virtual bool waitForSafeWindow(uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs) = 0;
            virtual void beginTimedWrite(size_t byteCount) = 0;
            virtual void endTimedWrite() = 0;
    };
}

#endif