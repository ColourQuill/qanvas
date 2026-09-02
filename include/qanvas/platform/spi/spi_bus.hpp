#ifndef __QANVAS_SPI_BUS_HPP__
#define __QANVAS_SPI_BUS_HPP__

// std
#include <cstdint>
#include <string>

namespace Qore::Qanvas {
    class Pin;
    class PinManager;

    class SPIBus {
        public:
            SPIBus(PinManager* pinManager, int dcPin, int rstPin, int blPin, const std::string& spiDevice, uint32_t speedHz);
            ~SPIBus();

            bool open();
            void reset();
            void setBacklight(bool on);

            void writeCommand(uint8_t cmd);
            void writeData(const uint8_t* data, size_t len);
            void writeData(uint8_t value);

            bool transferFullDuplex(const uint8_t* tx, uint8_t* rx, size_t len, uint32_t speedHz);

            size_t totalBytesSent() const;
            void resetBytesSent();
        private:
            void spiTransfer(const uint8_t* data, size_t len);

            PinManager* m_pinManager;
            Pin* m_dc = nullptr;
            Pin* m_rst = nullptr;
            Pin* m_bl = nullptr;

            int m_fd = -1;
            std::string m_spiDevice;
            uint32_t m_speedHz;
            size_t m_totalBytesSent = 0;
            
            static constexpr size_t MAX_TRANSFER = 4096;
    };
}

#endif