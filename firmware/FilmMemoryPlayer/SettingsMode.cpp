#include "SettingsMode.h"

#include <Fonts/TomThumb.h>

#include "AppState.h"
#include "BatteryMonitor.h"
#include "DisplayUtils.h"

namespace {
void drawCenteredHint(const String &text, int16_t baseline, uint16_t color) {
  tft.setFont(&TomThumb);
  tft.setTextSize(1);
  tft.setTextColor(color);
  int16_t x1, y1;
  uint16_t width, height;
  tft.getTextBounds(text, 0, baseline, &x1, &y1, &width, &height);
  tft.setCursor((DISPLAY_W - width) / 2 - x1, baseline);
  tft.print(text);
  tft.setFont();
}
} // namespace

void drawSettings() {
  tft.fillScreen(ST77XX_BLACK);
  drawCentered("SETTINGS", 8, 1, ST77XX_WHITE);
  const uint16_t active = ST77XX_ORANGE;
  const uint16_t inactive = ST77XX_WHITE;
  drawCentered(String(settingsSelection == 0 ? "> " : "  ") + "SLIDE " +
                   String(SLIDE_INTERVALS_MS[slideIntervalIndex] / 1000) + " SEC",
               28, 1, settingsSelection == 0 ? active : inactive);
  String sleepValue = AUTO_SLEEP_MINUTES[autoSleepIndex] == 0
                          ? "OFF"
                          : String(AUTO_SLEEP_MINUTES[autoSleepIndex]) + " MIN";
  drawCentered(String(settingsSelection == 1 ? "> " : "  ") + "SLEEP " + sleepValue,
               46, 1, settingsSelection == 1 ? active : inactive);
  drawCentered(String(settingsSelection == 2 ? "> " : "  ") + "BRIGHT " +
                   String(BRIGHTNESS_PERCENT[brightnessIndex]) + "%",
               64, 1, settingsSelection == 2 ? active : inactive);
  if (HAS_BATTERY_MONITOR) {
    float voltage = 0.0f;
    uint8_t percent = 0;
    String batteryText = "BAT --";
    if (readBatteryStatus(voltage, percent)) {
      batteryText = "BAT ";
      batteryText += String(percent) + "% " + String(voltage, 2) + "V";
    }
    drawCentered(batteryText, 82, 1, ST77XX_GREEN);
    drawCenteredHint("TURN: CHANGE", 106, ST77XX_CYAN);
    drawCenteredHint("PRESS: NEXT", 119, ST77XX_WHITE);
  } else {
    drawCenteredHint("TURN: CHANGE", 106, ST77XX_CYAN);
    drawCenteredHint("PRESS: NEXT", 119, ST77XX_WHITE);
  }
}

void saveSettings() {
  preferences.putUChar("slide", slideIntervalIndex);
  preferences.putUChar("sleep", autoSleepIndex);
  preferences.putUChar("bright", brightnessIndex);
}
