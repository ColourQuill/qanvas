#ifndef __QANVAS_PIN_MANAGER_HPP__
#define __QANVAS_PIN_MANAGER_HPP__

// std
#include <unordered_map>

// forward declarations
struct gpiod_chip;

namespace Qore::Qanvas {
    class Pin;

    class PinManager {
        public:
            ~PinManager();

            bool init(const char* chipPath);

            Pin* get(int number);
            void release(int number);
        private:
            gpiod_chip* m_chip = nullptr;
            std::unordered_map<int, Pin*> m_pins;
    };
}

#endif