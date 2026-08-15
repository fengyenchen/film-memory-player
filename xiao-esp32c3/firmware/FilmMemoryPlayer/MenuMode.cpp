#include "MenuMode.h"

#include "AppState.h"
#include "ClockMode.h"
#include "DisplayUtils.h"
#include "EmoteMode.h"
#include "PhotoMode.h"
#include "SettingsMode.h"
#include "WifiManager.h"

void drawMenu() {
  tft.fillScreen(ST77XX_BLACK);
  drawCentered("SELECT MODE", 12, 1, ST77XX_WHITE);
  const String label = String("<") + MENU_LABELS[menuSelection] + ">";
  drawCentered(label, 48, 2, ST77XX_ORANGE);
  drawCentered(String(menuSelection + 1) + " / " + String(MENU_ITEM_COUNT), 91, 1, ST77XX_CYAN);
  drawCentered("PRESS TO ENTER", 108, 1, ST77XX_WHITE);
}

void openMainMenu() {
  stopSetupServer();
  autoplay = false;
  appMode = AppMode::MENU;
  lastUserActionAt = millis();
  drawMenu();
}

void showCurrentMode() {
  switch (appMode) {
    case AppMode::PHOTO: drawPhoto(); break;
    case AppMode::CLOCK: drawClock(); break;
    case AppMode::EMOTES: drawExpression(); break;
    case AppMode::WIFI:
      if (!setupMode) startSetupServer();
      break;
    case AppMode::SETTINGS: drawSettings(); break;
    case AppMode::MENU: drawMenu(); break;
  }
}

void enterSelectedMode() {
  lastUserActionAt = millis();
  switch (menuSelection) {
    case 0:
      appMode = AppMode::PHOTO;
      drawPhoto();
      break;
    case 1:
      appMode = AppMode::CLOCK;
      drawClock();
      break;
    case 2:
      appMode = AppMode::EMOTES;
      drawExpression();
      break;
    case 3:
      startSetupServer();
      break;
    default:
      appMode = AppMode::SETTINGS;
      settingsSelection = 0;
      drawSettings();
      break;
  }
}
