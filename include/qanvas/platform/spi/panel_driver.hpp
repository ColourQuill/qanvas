#ifndef __QANVAS_PANEL_DRIVER_HPP__
#define __QANVAS_PANEL_DRIVER_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/physical_rect.hpp>

namespace Qore::Qanvas {
    class SPIBus;

    class PanelDriver {
        public:
            virtual ~PanelDriver() = default;

            virtual void init(SPIBus* bus) = 0;

            virtual uint16_t width() const = 0;
            virtual uint16_t height() const = 0;

            virtual void writeRect(SPIBus* bus, PhysicalRect rect, const uint16_t* pixels) = 0;
            virtual void defineScrollWindow(SPIBus* bus, uint16_t topFixed, uint16_t bottomFixed) = 0;
            virtual void setScrollOffset(SPIBus* bus, uint16_t line) = 0;
            virtual void setMirrored(SPIBus* bus, bool mirrored) = 0;
            virtual bool waitForSafeWindow(SPIBus* bus, uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs) = 0;
            virtual void beginTimedWrite(SPIBus* bus, size_t byteCount) = 0;
            virtual void endTimedWrite(SPIBus* bus) = 0;
    };
}

#endif