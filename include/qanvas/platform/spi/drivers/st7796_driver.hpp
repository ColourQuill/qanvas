#ifndef __QANVAS_ST7796_DRIVER_HPP__
#define __QANVAS_ST7796_DRIVER_HPP__

// std
#include <cstddef>

// platform
#include <qanvas/platform/spi/panel_driver.hpp>

namespace Qore::Qanvas {
    class ST7796Driver : public PanelDriver {
        public:
            ST7796Driver(uint16_t width, uint16_t height, uint32_t speedHz);

            void init(SPIBus* bus) override;

            uint16_t width() const override;
            uint16_t height() const override;

            void writeRect(SPIBus* bus, PhysicalRect rect, const uint16_t* pixels) override;
            void defineScrollWindow(SPIBus* bus, uint16_t topFixed, uint16_t bottomFixed) override;
            void setScrollOffset(SPIBus* bus, uint16_t line) override;
            void setMirrored(SPIBus* bus, bool mirrored) override;
            bool waitForSafeWindow(SPIBus* bus, uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs) override;
            void beginTimedWrite(SPIBus* bus, size_t byteCount) override;
            void endTimedWrite(SPIBus* bus) override;
        private:
            void setAddressWindow(SPIBus* bus, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
            void setExtendedCommands(SPIBus* bus, bool on);
            void writeMadctl(SPIBus* bus);
            void setBlankingPorch(SPIBus* bus, uint8_t vfp, uint8_t vbp, uint8_t hbp);
            void setFramerate(SPIBus* bus, uint8_t frs, uint8_t rtna);
            uint16_t getScanline(SPIBus* bus);
            bool waitForBlanking(SPIBus* bus, uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs);
            uint8_t porch(double ms) const;
            double estimateMs(double bytes) const;

            uint16_t m_width;
            uint16_t m_height;
            uint32_t m_speedHz;
            uint8_t m_madctl = 0;
            bool m_mirrored = false;

            static constexpr size_t MAX_TRANSFER = 4096;
    };
}

#endif