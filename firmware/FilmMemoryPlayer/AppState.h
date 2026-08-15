#pragma once

#include <Arduino.h>
#include <FS.h>
#include <Preferences.h>
#include <SPI.h>
#include <WebServer.h>
#include <Adafruit_ST7789.h>

#include "Config.h"

class FilmMemoryST7789 : public Adafruit_ST7789 {
public:
  FilmMemoryST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst)
      : Adafruit_ST7789(spiClass, cs, dc, rst) {}

  void use128x128ZeroOffset();
};

enum class AppMode : uint8_t {
  PHOTO,
  CLOCK,
  EMOTES,
  WIFI,
  SETTINGS,
  MENU
};

extern FilmMemoryST7789 tft;
extern WebServer server;
extern Preferences preferences;
extern uint16_t photoFrame[DISPLAY_W * DISPLAY_H];

extern const char *const MENU_LABELS[MENU_ITEM_COUNT];
extern AppMode appMode;
extern uint8_t menuSelection;
extern uint8_t settingsSelection;
extern uint8_t slideIntervalIndex;
extern uint8_t autoSleepIndex;
extern uint8_t currentExpression;
extern bool expressionSelectionDirty;
extern uint32_t expressionSelectionChangedAt;

extern String photos[MAX_PHOTOS];
extern uint8_t photoCount;
extern uint8_t currentPhoto;
extern bool fileSystemReady;
extern bool setupMode;
extern bool webRoutesConfigured;
extern bool autoplay;
extern bool sleeping;
extern uint32_t lastSlideAt;
extern uint32_t lastEncoderActionAt;
extern uint32_t lastUserActionAt;
extern uint32_t lastClockDrawAt;

extern uint8_t encoderPrevious;
extern int8_t encoderAccumulated;
extern bool buttonLastReading;
extern bool buttonStable;
extern uint32_t buttonChangedAt;
extern uint32_t buttonPressedAt;
extern bool menuHoldHandled;
extern bool longPressHandled;

extern File uploadFile;
extern bool uploadOK;
extern String uploadError;
