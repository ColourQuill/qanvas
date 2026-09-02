#include <qanvas/platform/spi/spi_bus.hpp>

// linux
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

// platform
#include <qanvas/platform/gpio/pin.hpp>
#include <qanvas/platform/gpio/pin_manager.hpp>

using namespace Qore::Qanvas;

SPIBus::SPIBus(PinManager* pinManager, int dcPin, int rstPin, int blPin, const std::string& spiDevice, uint32_t speedHz) : m_pinManager(pinManager), m_spiDevice(spiDevice), m_speedHz(speedHz) {
    m_dc = m_pinManager->get(dcPin);
    m_rst = m_pinManager->get(rstPin);
    m_bl = m_pinManager->get(blPin);
}

SPIBus::~SPIBus() {
    if (m_fd >= 0) close(m_fd);
}

bool SPIBus::open() {
    if (!m_dc->setOutput(false) || !m_rst->setOutput(true) || !m_bl->setOutput(true)) {
        return false;
    }

    m_fd = ::open(m_spiDevice.c_str(), O_RDWR);
    if (m_fd < 0) return false;

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    if (ioctl(m_fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_speedHz) < 0) {
        return false;
    }

    return true;
}

void SPIBus::reset() {
    m_rst->write(false);
    usleep(20 * 1000);
    m_rst->write(true);
    usleep(150 * 1000);
}

void SPIBus::setBacklight(bool on) {
    m_bl->write(on);
}

void SPIBus::writeCommand(uint8_t cmd) {
    m_dc->write(false);
    spiTransfer(&cmd, 1);
}

void SPIBus::writeData(const uint8_t* data, size_t len) {
    m_dc->write(true);
    size_t sent = 0;
    while (sent < len) {
        size_t chunk = (len - sent) < MAX_TRANSFER ? (len - sent) : MAX_TRANSFER;
        spiTransfer(data + sent, chunk);
        sent += chunk;
    }
}

void SPIBus::writeData(uint8_t value) {
    writeData(&value, 1);
}

bool SPIBus::transferFullDuplex(const uint8_t* tx, uint8_t* rx, size_t len, uint32_t speedHz) {
    struct spi_ioc_transfer tr{};
    tr.tx_buf = reinterpret_cast<uintptr_t>(tx);
    tr.rx_buf = reinterpret_cast<uintptr_t>(rx);
    tr.len = static_cast<uint32_t>(len);
    tr.speed_hz = speedHz;
    tr.bits_per_word = 8;
    return ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr) >= 0;
}

size_t SPIBus::totalBytesSent() const {
    return m_totalBytesSent;
}
void SPIBus::resetBytesSent() {
    m_totalBytesSent = 0;
}

void SPIBus::spiTransfer(const uint8_t* data, size_t len) {
    m_totalBytesSent += len;

    struct spi_ioc_transfer tr{};
    tr.tx_buf = reinterpret_cast<uint64_t>(data);
    tr.rx_buf = 0;
    tr.len = static_cast<uint32_t>(len);
    tr.speed_hz = 0;
    tr.bits_per_word = 8;
    ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr);
}