#include <qanvas/platform/spi/drivers/st7796_driver.hpp>

// std
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>

// linux
#include <unistd.h>

// platform
#include <qanvas/platform/spi/spi_bus.hpp>
#include <qanvas/platform/spi/commands/st7796_commands.hpp>

// utils
#include <utils/logger.hpp> // change to be qontext/utils/loggger.hpp (note)

using namespace Qore::Qanvas;

ST7796Driver::ST7796Driver(uint16_t width, uint16_t height, uint32_t speedHz) : m_width(width), m_height(height), m_speedHz(speedHz) {}

void ST7796Driver::init(SPIBus* bus) {
    bus->setBacklight(false);
    bus->reset();

    bus->writeCommand(ST7796_INTERFACE_PIXEL_FORMAT);
    bus->writeData(0x55);

    m_madctl = ST7796_MADCTL_RGB_BGR_ORDER | ST7796_MADCTL_COLUMN_ADDRESS_ORDER;
    writeMadctl(bus);

    bus->writeCommand(ST7796_DISPLAY_INVERSION_OFF);

    bus->writeCommand(ST7796_SLEEP_OUT);
    usleep(120 * 1000);
    bus->writeCommand(ST7796_DISPLAY_ON);
    usleep(20 * 1000);

    bus->setBacklight(true);
}

uint16_t ST7796Driver::width() const {
    return m_width;
}
uint16_t ST7796Driver::height() const {
    return m_height;
}

void ST7796Driver::setAddressWindow(SPIBus* bus, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    qtxt::Logger::logInfo("setAddressWindow()", "bus=" + qtxt::Logger::ptrToStr(bus) + " x0=" + std::to_string(x0) + " y0=" + std::to_string(y0) + " x1=" + std::to_string(x1) + " y1=" + std::to_string(y1));
    bus->writeCommand(ST7796_COLUMN_ADDRESS_SET);
    uint8_t colData[4] = {
        static_cast<uint8_t>(x0 >> 8), static_cast<uint8_t>(x0 & 0xFF),
        static_cast<uint8_t>(x1 >> 8), static_cast<uint8_t>(x1 & 0xFF)
    };
    bus->writeData(colData, sizeof(colData));

    bus->writeCommand(ST7796_ROW_ADDRESS_SET);
    uint8_t pageData[4] = {
        static_cast<uint8_t>(y0 >> 8), static_cast<uint8_t>(y0 & 0xFF),
        static_cast<uint8_t>(y1 >> 8), static_cast<uint8_t>(y1 & 0xFF)
    };
    bus->writeData(pageData, sizeof(pageData));

    bus->writeCommand(ST7796_MEMORY_WRITE);
}

void ST7796Driver::writeRect(SPIBus* bus, PhysicalRect rect, const uint16_t* pixels) {
    qtxt::Logger::logInfo("writeRect()", "bus=" + qtxt::Logger::ptrToStr(bus) + " rect={x=" + std::to_string(rect.x) + " y=" + std::to_string(rect.y) + " w=" + std::to_string(rect.w) + " h=" + std::to_string(rect.h) + "}"); 
    setAddressWindow(bus, (uint16_t)rect.x, (uint16_t)rect.y, (uint16_t)(rect.x + rect.w - 1), (uint16_t)(rect.y + rect.h - 1));

    size_t count = (size_t)rect.w * rect.h;
    std::vector<uint8_t> wire(count * 2);
    for (size_t i = 0; i < count; ++i) {
        wire[i * 2]     = (uint8_t)(pixels[i] >> 8);
        wire[i * 2 + 1] = (uint8_t)(pixels[i] & 0xFF);
    }
    bus->writeData(wire.data(), wire.size());
}

void ST7796Driver::defineScrollWindow(SPIBus* bus, uint16_t topFixed, uint16_t bottomFixed) {
    qtxt::Logger::logInfo("defineScrollWindow()", "bus=" + qtxt::Logger::ptrToStr(bus) + " topFixed=" + std::to_string(topFixed) + " bottomFixed=" + std::to_string(bottomFixed));
    uint16_t scrollHeight = m_height - topFixed - bottomFixed;
    bus->writeCommand(ST7796_VERTICAL_SCROLLING_DEFINITION);
    uint8_t data[6] = {
        (uint8_t)(topFixed >> 8), (uint8_t)(topFixed & 0xFF),
        (uint8_t)(scrollHeight >> 8), (uint8_t)(scrollHeight & 0xFF),
        (uint8_t)(bottomFixed >> 8), (uint8_t)(bottomFixed & 0xFF)
    };
    bus->writeData(data, sizeof(data));
}

void ST7796Driver::setScrollOffset(SPIBus* bus, uint16_t line) {
    qtxt::Logger::logInfo("setScrollOffset()", "bus=" + qtxt::Logger::ptrToStr(bus) + " line=" + std::to_string(line));
    bus->writeCommand(ST7796_VERTICAL_SCROLL_START_ADDRESS);
    uint8_t data[2] = { (uint8_t)(line >> 8), (uint8_t)(line & 0xFF) };
    bus->writeData(data, sizeof(data));
}

void ST7796Driver::setMirrored(SPIBus* bus, bool mirrored) {
    qtxt::Logger::logInfo("setMirrored()", "bus=" + qtxt::Logger::ptrToStr(bus) + " mirrored=" + std::to_string(mirrored));
    if (m_mirrored == mirrored) return;
    m_mirrored = mirrored;

    if (mirrored) m_madctl |= ST7796_MADCTL_ROW_ADDRESS_ORDER;
    else m_madctl &= ~ST7796_MADCTL_ROW_ADDRESS_ORDER;

    if (mirrored) m_madctl |= ST7796_MADCTL_VERTICAL_REFRESH_ORDER;
    else m_madctl &= ~ST7796_MADCTL_VERTICAL_REFRESH_ORDER;

    writeMadctl(bus);
}

void ST7796Driver::writeMadctl(SPIBus* bus) {
    qtxt::Logger::logInfo("writeMadctl()", "bus=" + qtxt::Logger::ptrToStr(bus));
    bus->writeCommand(ST7796_MEMORY_DATA_ACCESS_CONTROL);
    bus->writeData(m_madctl);
}

void ST7796Driver::setExtendedCommands(SPIBus* bus, bool on) {
    qtxt::Logger::logInfo("setExtendedCommands()", "bus=" + qtxt::Logger::ptrToStr(bus) + " on=" + std::to_string(on));
    bus->writeCommand(ST7796_COMMAND_SET_CONTROL);
    bus->writeData(on ? 0xC3 : 0x3C);
    bus->writeCommand(ST7796_COMMAND_SET_CONTROL);
    bus->writeData(on ? 0x96 : 0x69);
}

void ST7796Driver::setBlankingPorch(SPIBus* bus, uint8_t vfp, uint8_t vbp, uint8_t hbp) {
    qtxt::Logger::logInfo("setBlankingPorch()", "bus=" + qtxt::Logger::ptrToStr(bus) + " vfp=" + std::to_string(vfp) + " vbp=" + std::to_string(vbp) + " hbp=" + std::to_string(hbp));
    bus->writeCommand(ST7796_BLANKING_PORCH_CONTROL);
    uint8_t data[4] = { vfp, vbp, 0x00, hbp };
    bus->writeData(data, sizeof(data));
}

void ST7796Driver::setFramerate(SPIBus* bus, uint8_t frs, uint8_t rtna) {
    qtxt::Logger::logInfo("setFramerate()", "bus=" + qtxt::Logger::ptrToStr(bus) + " frs=" + std::to_string(frs) + " rtna=" + std::to_string(rtna));
    uint8_t p1 = (uint8_t)((frs & 0x0F) << 4);
    uint8_t p2 = (uint8_t)(rtna & 0x1F);
    bus->writeCommand(ST7796_FRAME_RATE_CONTROL);
    uint8_t data[2] = { p1, p2 };
    bus->writeData(data, sizeof(data));
}

uint16_t ST7796Driver::getScanline(SPIBus* bus) {
    uint8_t tx[4] = { 0x45, 0x00, 0x00, 0x00 };
    uint8_t rx[4] = { 0, 0, 0, 0 };
    if (!bus->transferFullDuplex(tx, rx, 4, 8000000)) return 0xFFFF;

    uint16_t hi = rx[3];
    uint16_t lo = rx[2];

    uint16_t scanline = (lo << 1) | (hi >> 7);
    return scanline;
}

bool ST7796Driver::waitForBlanking(SPIBus* bus, uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMS) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMS);
    while (std::chrono::steady_clock::now() < deadline) {
        uint16_t n = getScanline(bus);
        if (n == 0xFFFF) continue;
        if (n < vactStart || n >= vactEnd) return true;
    }
    return false;
}

bool ST7796Driver::waitForSafeWindow(SPIBus* bus, uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs) {
    return waitForBlanking(bus, vactStart, vactEnd, timeoutMs);
}

uint8_t ST7796Driver::porch(double ms) const {
    if (ms <= 0.0) return 2;
    constexpr double FIXED_PERIOD = 184.0;
    double totalLines = std::ceil((ms * 10000.0) / FIXED_PERIOD);
    if (totalLines <= 480.0) return 2;
    double extraLines = totalLines - 480.0;
    double porchEach = std::ceil(extraLines / 2.0);
    return (uint8_t)std::clamp<double>(porchEach, 2.0, 255.0);
}

double ST7796Driver::estimateMs(double bytes) const {
    double bits = bytes * 8.0;
    double spiMs = (bits / m_speedHz) * 1000.0;
    double buildMs = (bytes / 100000.0) * 1.2;
    double chunks = std::ceil(bytes / MAX_TRANSFER);
    double overheadMs = chunks * 0.1;
    return std::min<double>((spiMs + buildMs + overheadMs) * 1.15, 67.0); 
}

void ST7796Driver::beginTimedWrite(SPIBus* bus, size_t byteCount) {
    double ms = estimateMs((double)byteCount);
    uint8_t p = porch(ms);
    setExtendedCommands(bus, true);
    setFramerate(bus, 15, 20);
    setBlankingPorch(bus, p, p, 4);
    waitForBlanking(bus, 0, m_height, (uint32_t)(ms + 20.0));
}

void ST7796Driver::endTimedWrite(SPIBus* bus) {
    setBlankingPorch(bus, 2, 2, 4);
    setFramerate(bus, 10, 16);
    setExtendedCommands(bus, false);
}