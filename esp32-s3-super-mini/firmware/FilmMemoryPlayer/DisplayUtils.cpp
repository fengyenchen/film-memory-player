#include "DisplayUtils.h"

#include "AppState.h"

void setBacklight(bool on) {
  digitalWrite(PIN_TFT_BL, on ? HIGH : LOW);
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
