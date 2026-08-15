#include "SettingsMode.h"

#include "AppState.h"
#include "DisplayUtils.h"

void drawSettings() {
  tft.fillScreen(ST77XX_BLACK);
  drawCentered("SETTINGS", 8, 1, ST77XX_WHITE);
  const uint16_t active = ST77XX_ORANGE;
  const uint16_t inactive = ST77XX_WHITE;
  drawCentered(String(settingsSelection == 0 ? "> " : "  ") + "SLIDE " +
                   String(SLIDE_INTERVALS_MS[slideIntervalIndex] / 1000) + " SEC",
               38, 1, settingsSelection == 0 ? active : inactive);
  String sleepValue = AUTO_SLEEP_MINUTES[autoSleepIndex] == 0
                          ? "OFF"
                          : String(AUTO_SLEEP_MINUTES[autoSleepIndex]) + " MIN";
  drawCentered(String(settingsSelection == 1 ? "> " : "  ") + "SLEEP " + sleepValue,
               62, 1, settingsSelection == 1 ? active : inactive);
  drawCentered("TURN TO CHANGE", 94, 1, ST77XX_CYAN);
  drawCentered("PRESS: NEXT", 108, 1, ST77XX_WHITE);
}

void saveSettings() {
  preferences.putUChar("slide", slideIntervalIndex);
  preferences.putUChar("sleep", autoSleepIndex);
}
