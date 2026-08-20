/*
 * Film Memory Player
 *
 * The sketch entry point coordinates initialization and the main loop.
 * Feature implementations live in the adjacent .cpp/.h modules.
 */

#include <Arduino.h>
#include <LittleFS.h>
#include <SPI.h>
#include <TJpg_Decoder.h>
#include <driver/gpio.h>
#include <sys/time.h>
#include <time.h>

#include "AppState.h"
#include "BatteryMonitor.h"
#include "ClockMode.h"
#include "Config.h"
#include "DisplayUtils.h"
#include "Encoder.h"
#include "PhotoMode.h"
#include "PowerManager.h"

void setup() {
  Serial.begin(115200);
  delay(500);

  // 台灣時區；網頁會把手機／電腦的 Unix 時間同步給裝置。
  setenv("TZ", "CST-8", 1);
  tzset();
  preferences.begin("film-memory", false);
  slideIntervalIndex = preferences.getUChar("slide", 1);
  autoSleepIndex = preferences.getUChar("sleep", 0);
  brightnessIndex = preferences.getUChar("bright", BRIGHTNESS_OPTION_COUNT - 1);
  currentExpression = preferences.getUChar("emote", 0);
  if (slideIntervalIndex >= OPTION_COUNT) slideIntervalIndex = 1;
  if (autoSleepIndex >= OPTION_COUNT) autoSleepIndex = 0;
  if (brightnessIndex >= BRIGHTNESS_OPTION_COUNT) {
    brightnessIndex = BRIGHTNESS_OPTION_COUNT - 1;
  }
  if (currentExpression >= EXPRESSION_COUNT) currentExpression = 0;

  // 若前一次休眠鎖住背光腳，開機時先解除，再由程式正常控制。
  gpio_hold_dis(static_cast<gpio_num_t>(PIN_TFT_BL));
  pinMode(PIN_TFT_BL, OUTPUT);
  setBacklight(false); // 初始化完成前先關背光，避免看到白屏／雜訊

  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
  initBatteryMonitor();

  // 使用目前開發板的硬體 SPI，完整 128x128 畫面約十多毫秒送完。
  SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, PIN_TFT_CS);
  tft.init(DISPLAY_W, DISPLAY_H);
  tft.setSPISpeed(TFT_SPI_HZ);
  // 這塊 128x128 ST7735S 的可視記憶體從 (0,0) 開始。
  // 覆寫 Adafruit 預設的置中位移，確保繪圖座標對齊 (0,0) 至 (127,127)。
  tft.use128x128ZeroOffset();
  tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);
  tft.sendCommand(0x36, (const uint8_t[]){0x08}, 1); // BGR 色彩修正
  setBacklight(true);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(false);
  TJpgDec.setCallback(tftJpegOutput);

  encoderPrevious = (digitalRead(PIN_ENCODER_A) << 1) | digitalRead(PIN_ENCODER_B);
  buttonLastReading = buttonStable = digitalRead(PIN_ENCODER_SW);
  if (buttonStable == LOW) buttonPressedAt = millis();

  fileSystemReady = LittleFS.begin(true);
  if (!fileSystemReady) {
    showMessage("FS ERROR", "Formatting...");
    LittleFS.format();
    LittleFS.begin(true);
  }
  LittleFS.mkdir("/photos");
  scanPhotos();

  appMode = AppMode::PHOTO;
  lastUserActionAt = millis();
  drawPhoto();
}

void loop() {
  pollButton();

  if (setupMode) {
    server.handleClient();
    delay(2);
    return;
  }

  pollEncoder();
  const uint32_t now = millis();

  if (expressionSelectionDirty && now - expressionSelectionChangedAt >= 750) {
    preferences.putUChar("emote", currentExpression);
    expressionSelectionDirty = false;
  }

  if (appMode == AppMode::CLOCK && now - lastClockDrawAt >= 1000) {
    drawClock();
  }

  if (appMode == AppMode::PHOTO && autoplay && photoCount > 1 &&
      now - lastSlideAt >= SLIDE_INTERVALS_MS[slideIntervalIndex]) {
    currentPhoto = (currentPhoto + 1) % photoCount;
    lastSlideAt = now;
    drawPhoto();
  }

  const uint16_t sleepMinutes = AUTO_SLEEP_MINUTES[autoSleepIndex];
  if (sleepMinutes > 0 && now - lastUserActionAt >= uint32_t(sleepMinutes) * 60000UL) {
    enterSleep();
  }
  delay(1);
}
