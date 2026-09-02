#include <qanvas/platform/spi/spi_transport.hpp>

// platform
#include <qanvas/platform/spi/spi_bus.hpp>
#include <qanvas/platform/spi/panel_driver.hpp>

using namespace Qore::Qanvas;

SPITransport::SPITransport(SPIBus* bus, PanelDriver* driver) : m_bus(bus), m_driver(driver) {
    m_driver->init(m_bus);
}

uint16_t SPITransport::height() const {
    return m_driver->height();
}

void SPITransport::writeRect(PhysicalRect rect, const uint16_t* pixels) {
    m_driver->writeRect(m_bus, rect, pixels);
}
void SPITransport::defineScrollWindow(uint16_t topFixed, uint16_t bottomFixed) {
    m_driver->defineScrollWindow(m_bus, topFixed, bottomFixed);
}
void SPITransport::setScrollOffset(uint16_t line) {
    m_driver->setScrollOffset(m_bus, line);
}
void SPITransport::setMirrored(bool mirrored) {
    m_driver->setMirrored(m_bus, mirrored);
}
bool SPITransport::waitForSafeWindow(uint16_t vactStart, uint16_t vactEnd, uint32_t timeoutMs) {
    return m_driver->waitForSafeWindow(m_bus, vactStart, vactEnd, timeoutMs);
}
void SPITransport::beginTimedWrite(size_t byteCount) {
    m_driver->beginTimedWrite(m_bus, byteCount);
}
void SPITransport::endTimedWrite() {
    m_driver->endTimedWrite(m_bus);
}