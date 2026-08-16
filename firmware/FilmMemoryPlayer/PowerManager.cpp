#include "PowerManager.h"

#include <WiFi.h>
#include <driver/gpio.h>
#include <esp_sleep.h>

#include "AppState.h"
#include "DisplayUtils.h"

void enterSleep() {
  autoplay = false;
  sleeping = true;
  if (setupMode) {
    server.stop();
    setupMode = false;
  }
  WiFi.mode(WIFI_OFF);
  tft.fillScreen(ST77XX_BLACK);
  tft.enableDisplay(false);
  tft.enableSleep(true);
  delay(10);
  setBacklight(false);
  // 鎖住背光 GPIO 的 LOW，避免進入 Light-sleep 後腳位浮動、背光自行亮起。
  gpio_hold_en(static_cast<gpio_num_t>(PIN_TFT_BL));
  delay(80);

  // 休眠門檻是在按鍵仍為 LOW 時觸發。等本次長按放開並穩定為 HIGH，
  // 才允許下一次新的 LOW 作為喚醒事件。
  while (digitalRead(PIN_ENCODER_SW) == LOW) delay(10);
  uint32_t releasedAt = millis();
  while (millis() - releasedAt < 200) {
    if (digitalRead(PIN_ENCODER_SW) == LOW) releasedAt = millis();
    delay(5);
  }

  // 清除可能殘留的喚醒來源／GPIO 狀態，再設定 Encoder SW 低電位喚醒。
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  gpio_wakeup_disable(static_cast<gpio_num_t>(PIN_ENCODER_SW));
  const esp_err_t gpioResult =
      gpio_wakeup_enable(static_cast<gpio_num_t>(PIN_ENCODER_SW), GPIO_INTR_LOW_LEVEL);
  const esp_err_t sourceResult = esp_sleep_enable_gpio_wakeup();
  Serial.printf("Sleep setup: gpio=%d source=%d\n", gpioResult, sourceResult);
  Serial.flush();

  esp_err_t sleepResult = ESP_FAIL;
  uint32_t sleepStartedAt = millis();
  if (gpioResult == ESP_OK && sourceResult == ESP_OK) {
    sleepResult = esp_light_sleep_start();
    Serial.printf("Light-sleep returned: %d, cause=%d\n", sleepResult,
                  esp_sleep_get_wakeup_cause());
  }

  gpio_wakeup_disable(static_cast<gpio_num_t>(PIN_ENCODER_SW));
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  const uint32_t sleptForMs = millis() - sleepStartedAt;

  // 若核心拒絕睡眠或被殘留狀態提前喚醒，仍保持背光關閉，
  // 等待使用者下一次明確按下，避免裝置自行變亮。
  if (sleepResult != ESP_OK ||
      esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_GPIO ||
      sleptForMs < 300) {
    while (digitalRead(PIN_ENCODER_SW) == HIGH) delay(10);
  }
  while (digitalRead(PIN_ENCODER_SW) == LOW) delay(10);

  // Light-sleep 後重新啟動，確保 TFT、LittleFS、Encoder 全部重新初始化。
  gpio_hold_dis(static_cast<gpio_num_t>(PIN_TFT_BL));
  digitalWrite(PIN_TFT_BL, LOW);
  ESP.restart();
}
