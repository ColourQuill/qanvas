#ifndef __QANVAS_SPI_TRANSPORT_HPP__
#define __QANVAS_SPI_TRANSPORT_HPP__

// std
#include <cstddef>

// device
#include <qanvas/device/transport.hpp>

namespace Qore::Qanvas {
    class SPIBus;
    class PanelDriver;

    class SPITransport : public Transport {
        public:
            SPITransport(SPIBus* bus, PanelDriver* driver);

            uint16_t height() const override;

            void writeRect(PhysicalRect rect, const uint16_t* pixels) override;
            void defineScrollWindow(uint16_t topFixed, uint16_t bottomFixed) override;
            void setScrollOffset(uint16_t line) override;
            void setMirrored(bool mirrored) override;
            bool waitForSafeWindow(uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs) override;
            void beginTimedWrite(size_t byteCount) override;
            void endTimedWrite() override;
        private:
            SPIBus* m_bus;
            PanelDriver* m_driver;
    };
}

#endif