#include <qanvas/platform/gpio/pin.hpp>

// gpiod
#include <gpiod.h>
#include <poll.h>

using namespace Qore::Qanvas;

Pin::Pin(gpiod_chip* chip, unsigned int offset) : m_chip(chip), m_offset(offset) {}

Pin::~Pin() {
    unwatch();
    release();
}

void Pin::release() {
    if (m_eventBuffer) {
        gpiod_edge_event_buffer_free(m_eventBuffer);
        m_eventBuffer = nullptr;
    }
    if (m_lineRequest) {
        gpiod_line_request_release(m_lineRequest);
        m_lineRequest = nullptr;
    }
}

bool Pin::request(bool asInput, bool outputValue, bool pullUp, PinEdge* edge) {
    release();

    gpiod_line_settings* settings = gpiod_line_settings_new();
    if (!settings) return false;

    if (asInput) {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
        if (pullUp) gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
        if (edge) {
            gpiod_line_edge edgeValue = GPIOD_LINE_EDGE_NONE;
            switch (*edge) {
                case PinEdge::RISING: edgeValue = GPIOD_LINE_EDGE_RISING; break;
                case PinEdge::FALLING: edgeValue = GPIOD_LINE_EDGE_FALLING; break;
                case PinEdge::BOTH: edgeValue = GPIOD_LINE_EDGE_BOTH; break;
            }
            gpiod_line_settings_set_edge_detection(settings, edgeValue);
        }
    } else {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
        gpiod_line_settings_set_output_value(settings, outputValue ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
    }

    gpiod_line_config* lineConfig = gpiod_line_config_new();
    unsigned int offsets[1] = { m_offset };
    gpiod_line_config_add_line_settings(lineConfig, offsets, 1, settings);

    gpiod_request_config* reqConfig = gpiod_request_config_new();
    gpiod_request_config_set_consumer(reqConfig, "Qanvas");

    m_lineRequest = gpiod_chip_request_lines(m_chip, reqConfig, lineConfig);

    gpiod_request_config_free(reqConfig);
    gpiod_line_config_free(lineConfig);
    gpiod_line_settings_free(settings);

    return m_lineRequest != nullptr;
}

bool Pin::setInput(bool pullUp) {
    return request(true, false, pullUp, nullptr);
}
bool Pin::setOutput(bool initialValue) {
    return request(false, initialValue, false, nullptr);
}

bool Pin::read() const {
    if (!m_lineRequest) return false;
    return gpiod_line_request_get_value(m_lineRequest, m_offset) == GPIOD_LINE_VALUE_ACTIVE;
}

void Pin::write(bool value) {
    if (!m_lineRequest) return;
    gpiod_line_request_set_value(m_lineRequest, m_offset, value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
}

bool Pin::watch(PinEdge edge, std::function<void(bool)> callback, std::chrono::milliseconds debounce) {
    unwatch();
    if (!request(true, false, false, &edge)) return false;

    m_eventBuffer = gpiod_edge_event_buffer_new(16);
    if (!m_eventBuffer) {
        release();
        return false;
    }

    m_onChange = std::move(callback);
    m_debounceWindow = debounce;
    m_watching = true;
    m_watchThread = std::thread(&Pin::loop, this);
    return true;
}

void Pin::unwatch() {
    if (!m_watching) return;
    m_watching = false;
    if (m_watchThread.joinable()) m_watchThread.join();
}

void Pin::loop() {
    int fd = gpiod_line_request_get_fd(m_lineRequest);
    if (fd < 0) return;

    bool hasPending = false;
    bool pendingValue = false;

    while (m_watching) {
        int timeoutMs = hasPending ? (int)m_debounceWindow.count() : 200;

        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;

        int pollResult = poll(&pfd, 1, timeoutMs);

        if (pollResult <= 0) {
            if (hasPending) {
                if (m_onChange) m_onChange(pendingValue);
                hasPending = false;
            }
            continue;
        }

        int readResult = gpiod_line_request_read_edge_events(m_lineRequest, m_eventBuffer, 16);
        if (readResult <= 0) continue;

        for (int i = 0; i < readResult; ++i) {
            gpiod_edge_event* event = gpiod_edge_event_buffer_get_event(m_eventBuffer, i);
            if (!event) continue;

            bool value = (gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE);

            if (m_debounceWindow.count() == 0) {
                if (m_onChange) m_onChange(value);
            } else {
                pendingValue = value;
                hasPending = true;
            }
        }
    }
}

int Pin::get() const {
    return static_cast<int>(m_offset);
}