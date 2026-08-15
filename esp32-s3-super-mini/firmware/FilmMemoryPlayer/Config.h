#pragma once

#include <Arduino.h>

// ESP32-S3 Super Mini pins
constexpr int PIN_TFT_BL = 1;
constexpr int PIN_TFT_RST = 2;
constexpr int PIN_TFT_DC = 4;
constexpr int PIN_TFT_CS = 5;
constexpr int PIN_ENCODER_A = 6;
constexpr int PIN_ENCODER_B = 7;
constexpr int PIN_ENCODER_SW = 8;
constexpr int PIN_TFT_SCK = 9;
constexpr int PIN_TFT_MOSI = 10;

constexpr uint16_t DISPLAY_W = 128;
constexpr uint16_t DISPLAY_H = 128;
constexpr uint32_t TFT_SPI_HZ = 27000000;
constexpr uint8_t ENCODER_STEPS_PER_DETENT = 4;
constexpr uint32_t DEBOUNCE_MS = 30;
constexpr uint32_t MENU_HOLD_MS = 1200;
constexpr uint32_t POWER_OFF_HOLD_MS = 3500;
constexpr uint32_t BUTTON_AFTER_ROTATION_GUARD_MS = 500;
constexpr uint8_t MAX_PHOTOS = 50;

constexpr uint32_t SLIDE_INTERVALS_MS[] = {2000, 3500, 5000, 10000, 30000};
constexpr uint16_t AUTO_SLEEP_MINUTES[] = {0, 1, 3, 5};
constexpr uint8_t OPTION_COUNT = 4;

constexpr char AP_SSID[] = "FilmMemory-Setup";
constexpr char AP_PASSWORD[] = "film2026";

constexpr uint8_t MENU_ITEM_COUNT = 5;
constexpr uint8_t EXPRESSION_COUNT = 10;
