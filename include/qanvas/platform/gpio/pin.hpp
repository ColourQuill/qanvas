#ifndef __QANVAS_PIN_HPP__
#define __QANVAS_PIN_HPP__

// std
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdint>

// forward declarations
struct gpiod_chip;
struct gpiod_line_request;
struct gpiod_edge_event_buffer;

namespace Qore::Qanvas {
    enum class PinEdge {
        RISING,
        FALLING,
        BOTH
    };

    class Pin {
        public:
            ~Pin();

            bool setInput(bool pullUp = false);
            bool setOutput(bool initialValue = false);

            bool read() const;
            void write(bool value);

            bool watch(PinEdge edge, std::function<void(bool value)> callback, std::chrono::milliseconds debounce = std::chrono::milliseconds(0));
            void unwatch();

            int get() const;
        private:
            friend class PinManager;

            Pin(gpiod_chip* chip, unsigned int offset);

            gpiod_chip* m_chip = nullptr;
            unsigned int m_offset = 0;

            gpiod_line_request* m_lineRequest = nullptr;
            gpiod_edge_event_buffer* m_eventBuffer = nullptr;

            std::thread m_watchThread;
            std::atomic<bool> m_watching{false};
            std::function<void(bool)> m_onChange;
            std::chrono::milliseconds m_debounceWindow{0};

            bool request(bool asInput, bool outputValue, bool pullUp, PinEdge* edge);
            void release();
            void loop();
    };
}

#endif