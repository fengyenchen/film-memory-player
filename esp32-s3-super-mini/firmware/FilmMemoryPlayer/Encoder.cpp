#include "Encoder.h"

#include "AppState.h"
#include "Config.h"
#include "EmoteMode.h"
#include "MenuMode.h"
#include "PhotoMode.h"
#include "PowerManager.h"
#include "SettingsMode.h"
#include "WifiManager.h"

void pollEncoder() {
  const uint8_t state = (digitalRead(PIN_ENCODER_A) << 1) | digitalRead(PIN_ENCODER_B);
  if (state != encoderPrevious) {
    lastEncoderActionAt = millis();
    lastUserActionAt = millis();
  }
  const uint8_t transition = (encoderPrevious << 2) | state;
  static const int8_t table[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
  encoderAccumulated += table[transition & 0x0F];
  encoderPrevious = state;

  int8_t direction = 0;
  if (encoderAccumulated >= ENCODER_STEPS_PER_DETENT) {
    encoderAccumulated = 0;
    direction = 1;
  } else if (encoderAccumulated <= -ENCODER_STEPS_PER_DETENT) {
    encoderAccumulated = 0;
    direction = -1;
  }
  if (direction == 0) return;

  switch (appMode) {
    case AppMode::PHOTO:
      changePhoto(direction);
      break;
    case AppMode::MENU:
      menuSelection = (menuSelection + MENU_ITEM_COUNT + direction) % MENU_ITEM_COUNT;
      drawMenu();
      break;
    case AppMode::EMOTES:
      changeExpression(direction);
      break;
    case AppMode::SETTINGS:
      if (settingsSelection == 0) {
        slideIntervalIndex = (slideIntervalIndex + OPTION_COUNT + direction) % OPTION_COUNT;
      } else {
        autoSleepIndex = (autoSleepIndex + OPTION_COUNT + direction) % OPTION_COUNT;
      }
      saveSettings();
      drawSettings();
      break;
    default:
      break;
  }
}

void pollButton() {
  const bool reading = digitalRead(PIN_ENCODER_SW);
  uint32_t now = millis();
  if (reading != buttonLastReading) buttonChangedAt = now;

  if (now - buttonChangedAt > DEBOUNCE_MS && reading != buttonStable) {
    buttonStable = reading;
    if (buttonStable == LOW) {
      buttonPressedAt = now;
      lastUserActionAt = now;
      menuHoldHandled = false;
      longPressHandled = false;
    } else if (!menuHoldHandled && !longPressHandled) {
      lastUserActionAt = now;
      if (appMode == AppMode::WIFI || setupMode) {
        stopSetupServer();
        appMode = AppMode::MENU;
        scanPhotos();
        drawMenu();
      } else if (appMode == AppMode::MENU) {
        enterSelectedMode();
      } else if (appMode == AppMode::PHOTO) {
        if (now - lastEncoderActionAt >= BUTTON_AFTER_ROTATION_GUARD_MS) {
          autoplay = !autoplay;
          lastSlideAt = millis();
          Serial.printf("Slideshow: %s\n", autoplay ? "ON" : "OFF");
        }
      } else if (appMode == AppMode::SETTINGS) {
        settingsSelection = (settingsSelection + 1) % 2;
        drawSettings();
      }
    }
  }

  // 長按 1.2 秒開啟主選單；從任一模式都使用同一個返回手勢。
  if (buttonStable == LOW && !menuHoldHandled &&
      (now - buttonPressedAt >= MENU_HOLD_MS)) {
    menuHoldHandled = true;
    if (appMode != AppMode::MENU) openMainMenu();
  }

  // 繼續按住到 3.5 秒才休眠關機。
  if (buttonStable == LOW && !longPressHandled &&
      (now - buttonPressedAt >= POWER_OFF_HOLD_MS)) {
    longPressHandled = true;
    enterSleep();
  }
  buttonLastReading = reading;
}
