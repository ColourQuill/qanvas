#include <qanvas/platform/gpio/pin_manager.hpp>

// gpiod
#include <gpiod.h>

// platform
#include <qanvas/platform/gpio/pin.hpp>

using namespace Qore::Qanvas;

PinManager::~PinManager() {
    for (auto& [num, pin] : m_pins) delete pin;
    m_pins.clear();
    if (m_chip) gpiod_chip_close(m_chip);
}

bool PinManager::init(const char* chipPath) {
    m_chip = gpiod_chip_open(chipPath);
    return m_chip != nullptr;
}

Pin* PinManager::get(int number) {
    auto it = m_pins.find(number);
    if (it != m_pins.end()) return it->second;

    Pin* pin = new Pin(m_chip, static_cast<unsigned int>(number));
    m_pins[number] = pin;
    return pin;
}

void PinManager::release(int number) {
    auto it = m_pins.find(number);
    if (it == m_pins.end()) return;
    delete it->second;
    m_pins.erase(it);
}