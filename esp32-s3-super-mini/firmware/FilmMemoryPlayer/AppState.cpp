#include "AppState.h"

void FilmMemoryST7789::use128x128ZeroOffset() {
  setColRowStart(0, 0);
  setRotation(0);
}

FilmMemoryST7789 tft(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
WebServer server(80);
Preferences preferences;
uint16_t photoFrame[DISPLAY_W * DISPLAY_H];

const char *const MENU_LABELS[MENU_ITEM_COUNT] = {
    "PHOTO", "CLOCK", "EMOTES", "WI-FI", "SETTINGS"};
AppMode appMode = AppMode::PHOTO;
uint8_t menuSelection = 0;
uint8_t settingsSelection = 0;
uint8_t slideIntervalIndex = 1;
uint8_t autoSleepIndex = 0;
uint8_t currentExpression = 0;
bool expressionSelectionDirty = false;
uint32_t expressionSelectionChangedAt = 0;

String photos[MAX_PHOTOS];
uint8_t photoCount = 0;
uint8_t currentPhoto = 0;
bool fileSystemReady = false;
bool setupMode = false;
bool webRoutesConfigured = false;
bool autoplay = false;
bool sleeping = false;
uint32_t lastSlideAt = 0;
uint32_t lastEncoderActionAt = 0;
uint32_t lastUserActionAt = 0;
uint32_t lastClockDrawAt = 0;

uint8_t encoderPrevious = 0;
int8_t encoderAccumulated = 0;
bool buttonLastReading = HIGH;
bool buttonStable = HIGH;
uint32_t buttonChangedAt = 0;
uint32_t buttonPressedAt = 0;
bool menuHoldHandled = false;
bool longPressHandled = false;

File uploadFile;
bool uploadOK = false;
String uploadError;
