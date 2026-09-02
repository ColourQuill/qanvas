#ifndef __QANVAS_ST7796_COMMANDS_HPP__
#define __QANVAS_ST7796_COMMANDS_HPP__

// std
#include <cstdint>

// ================================================================================
// https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf
// ================================================================================

// =================================== COMMANDS ===================================

constexpr uint8_t ST7796_SOFTWARE_RESET = 0x01;
constexpr uint8_t ST7796_READ_DISPLAY_ID = 0x04;
constexpr uint8_t ST7796_READ_NUM_ERRORS = 0x05;
constexpr uint8_t ST7796_READ_DISPLAY_STATUS = 0x09;
constexpr uint8_t ST7796_READ_DISPLAY_POWER_MODE = 0x0A;
constexpr uint8_t ST7796_READ_DISPLAY_MADCTL = 0x0B;
constexpr uint8_t ST7796_READ_DISPLAY_PIXEL_FORMAT = 0x0C;
constexpr uint8_t ST7796_READ_DISPLAY_IMAGE_MODE = 0x0D;
constexpr uint8_t ST7796_READ_DISPLAY_SIGNAL_MODE = 0x0E;
constexpr uint8_t ST7796_READ_DISPLAY_DIAGNOSTIC_RESULT = 0x0F;
constexpr uint8_t ST7796_SLEEP_IN = 0x10;
constexpr uint8_t ST7796_SLEEP_OUT = 0x11;
constexpr uint8_t ST7796_PARTIAL_DISPLAY_MODE_ON = 0x12;
constexpr uint8_t ST7796_NORMAL_DISPLAY_MODE_ON = 0x13;
constexpr uint8_t ST7796_DISPLAY_INVERSION_OFF = 0x20;
constexpr uint8_t ST7796_DISPLAY_INVERSION_ON = 0x21;
constexpr uint8_t ST7796_DISPLAY_OFF = 0x28;
constexpr uint8_t ST7796_DISPLAY_ON = 0x29;
constexpr uint8_t ST7796_COLUMN_ADDRESS_SET = 0x2A;
constexpr uint8_t ST7796_ROW_ADDRESS_SET = 0x2B;
constexpr uint8_t ST7796_MEMORY_WRITE = 0x2C;
constexpr uint8_t ST7796_MEMORY_READ = 0x2E;
constexpr uint8_t ST7796_PARTIAL_AREA = 0x30;
constexpr uint8_t ST7796_VERTICAL_SCROLLING_DEFINITION = 0x33;
constexpr uint8_t ST7796_TEARING_EFFECT_LINE_OFF = 0x34;
constexpr uint8_t ST7796_TEARING_EFFECT_LINE_ON = 0x35;
constexpr uint8_t ST7796_MEMORY_DATA_ACCESS_CONTROL = 0x36;
constexpr uint8_t ST7796_VERTICAL_SCROLL_START_ADDRESS = 0x37;
constexpr uint8_t ST7796_IDLE_MODE_OFF = 0x38;
constexpr uint8_t ST7796_IDLE_MODE_ON = 0x39;
constexpr uint8_t ST7796_INTERFACE_PIXEL_FORMAT = 0x3A;
constexpr uint8_t ST7796_WRITE_MEMORY_CONTINUE = 0x3C;
constexpr uint8_t ST7796_READ_MEMORY_CONTINUE = 0x3E;
constexpr uint8_t ST7796_SET_TEAR_SCANLINE = 0x44;
constexpr uint8_t ST7796_GET_SCANLINE = 0x45;
constexpr uint8_t ST7796_WRITE_DISPLAY_BRIGHTNESS = 0x51;
constexpr uint8_t ST7796_READ_DISPLAY_BRIGHTNESS = 0x52;
constexpr uint8_t ST7796_WRITE_CONTROL_DISPLAY = 0x53;
constexpr uint8_t ST7796_READ_CONTROL_DISPLAY = 0x54;
constexpr uint8_t ST7796_WRITE_ADAPTIVE_BRIGHTNESS_CONTROL = 0x55;
constexpr uint8_t ST7796_READ_ADAPTIVE_BRIGHTNESS_CONTROL = 0x56;
constexpr uint8_t ST7796_WRITE_CABC_MINIMUM_BRIGHTNESS = 0x5E;
constexpr uint8_t ST7796_READ_CABC_MINIMUM_BRIGHTNESS = 0x5F;
constexpr uint8_t ST7796_READ_FIRST_CHECKSUM = 0xAA;
constexpr uint8_t ST7796_READ_CONTINUE_CHECKSUM = 0xAF;
constexpr uint8_t ST7796_READ_ID_1 = 0xDA;
constexpr uint8_t ST7796_READ_ID_2 = 0xDB;
constexpr uint8_t ST7796_READ_ID_3 = 0xDC;
constexpr uint8_t ST7796_INTERFACE_MODE_CONTROL = 0xB0;
constexpr uint8_t ST7796_FRAME_RATE_CONTROL = 0xB1;
constexpr uint8_t ST7796_FRAME_RATE_CONTROL_2 = 0xB2;
constexpr uint8_t ST7796_FRAME_RATE_CONTROL_3 = 0xB3;
constexpr uint8_t ST7796_DISPLAY_INVERSION_CONTROL = 0xB4;
constexpr uint8_t ST7796_BLANKING_PORCH_CONTROL = 0xB5;
constexpr uint8_t ST7796_DISPLAY_FUNCTION_CONTROL = 0xB6;
constexpr uint8_t ST7796_ENTRY_MODE_SET = 0xB7;
constexpr uint8_t ST7796_POWER_CONTROL_1 = 0xC0;
constexpr uint8_t ST7796_POWER_CONTROL_2 = 0xC1;
constexpr uint8_t ST7796_POWER_CONTROL_3 = 0xC2;
constexpr uint8_t ST7796_VCOM_CONTROL = 0xC5;
constexpr uint8_t ST7796_VCOM_OFFSET_REGISTER = 0xC6;
constexpr uint8_t ST7796_NVM_ADDRESS_DATA_WRITE = 0xD0;
constexpr uint8_t ST7796_NVM_BYTE_PROGRAM = 0xD1;
constexpr uint8_t ST7796_NVM_STATUS_READ = 0xD2;
constexpr uint8_t ST7796_READ_ID_4 = 0xD3;
constexpr uint8_t ST7796_POSITIVE_GAMMA_CONTROL = 0xE0;
constexpr uint8_t ST7796_NEGATIVE_GAMMA_CONTROL = 0xE1;
constexpr uint8_t ST7796_DIGITAL_GAMMA_CONTROL_1 = 0xE2;
constexpr uint8_t ST7796_DIGITAL_GAMMA_CONTROL_2 = 0xE3;
constexpr uint8_t ST7796_DISPLAY_OUTPUT_CONTROL_ADJUST = 0xE8;
constexpr uint8_t ST7796_COMMAND_SET_CONTROL = 0xF0;
constexpr uint8_t ST7796_SPI_READ_CONTROL = 0xFB;

// ======================= MEMORY DATA ACCESS CONTROL BITS ========================

constexpr uint8_t ST7796_MADCTL_ROW_ADDRESS_ORDER = 0x80;
constexpr uint8_t ST7796_MADCTL_COLUMN_ADDRESS_ORDER = 0x40;
constexpr uint8_t ST7796_MADCTL_ROW_COLUMN_EXCHANGE = 0x20;
constexpr uint8_t ST7796_MADCTL_VERTICAL_REFRESH_ORDER = 0x10;
constexpr uint8_t ST7796_MADCTL_RGB_BGR_ORDER = 0x08;
constexpr uint8_t ST7796_MADCTL_HORIZONTAL_REFRESH_ORDER = 0x04;

// ================================================================================

#endif