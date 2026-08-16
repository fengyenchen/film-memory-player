#include "DisplayUtils.h"

#include <driver/gpio.h>
#include <esp_arduino_version.h>

#include "AppState.h"

namespace {
bool backlightPwmAttached = false;

void detachBacklightPwm() {
  if (!backlightPwmAttached) return;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcDetach(PIN_TFT_BL);
#else
  ledcDetachPin(PIN_TFT_BL);
#endif
  backlightPwmAttached = false;
}
} // namespace

void setBacklight(bool on) {
  if (!on) {
    detachBacklightPwm();
    const gpio_num_t backlightPin = static_cast<gpio_num_t>(PIN_TFT_BL);
    gpio_set_direction(backlightPin, GPIO_MODE_OUTPUT);
    gpio_set_drive_capability(backlightPin, GPIO_DRIVE_CAP_3);
    gpio_set_level(backlightPin, 0);
    gpio_set_pull_mode(backlightPin, GPIO_PULLDOWN_ONLY);
    return;
  }

  gpio_set_pull_mode(static_cast<gpio_num_t>(PIN_TFT_BL), GPIO_FLOATING);
  const uint8_t duty = BRIGHTNESS_LEVELS[brightnessIndex];
  if (duty == 255) {
    detachBacklightPwm();
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
    return;
  }

  analogWrite(PIN_TFT_BL, duty);
  backlightPwmAttached = true;
}

void drawCentered(const String &text, int16_t y, uint8_t size, uint16_t color) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  tft.setCursor((DISPLAY_W - w) / 2, y);
  tft.print(text);
}

void showMessage(const String &title, const String &detail) {
  tft.fillScreen(ST77XX_BLACK);
  drawCentered(title, 20, 1, ST77XX_WHITE);
  if (detail.length()) drawCentered(detail, 45, 1, ST77XX_CYAN);
}
