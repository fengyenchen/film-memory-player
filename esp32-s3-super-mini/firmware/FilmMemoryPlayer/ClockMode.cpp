#include "ClockMode.h"

#include <time.h>

#include "AppState.h"
#include "DisplayUtils.h"

void drawClock() {
  lastClockDrawAt = millis();
  const time_t now = time(nullptr);
  if (now < 1700000000) {
    showMessage("CLOCK NOT SET", "OPEN WI-FI");
    return;
  }

  struct tm localTime;
  localtime_r(&now, &localTime);
  char timeText[9];
  char dateText[11];
  static const char *WEEKDAYS[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
  strftime(timeText, sizeof(timeText), "%H:%M:%S", &localTime);
  strftime(dateText, sizeof(dateText), "%Y-%m-%d", &localTime);
  tft.fillScreen(ST77XX_BLACK);
  drawCentered(timeText, 35, 2, ST77XX_WHITE);
  drawCentered(dateText, 72, 1, ST77XX_CYAN);
  drawCentered(WEEKDAYS[localTime.tm_wday], 108, 1, ST77XX_ORANGE);
}
