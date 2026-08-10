#include <Arduino.h>

constexpr int TEST_PIN = 21;
constexpr uint32_t DEBOUNCE_MS = 30;

bool lastReading = HIGH;
bool stableState = HIGH;
uint32_t changedAt = 0;

void printState(bool state) {
  if (state == LOW) {
    Serial.println(">>> 按鈕已按下 (LOW)");
  } else {
    Serial.println(">>> 按鈕已放開 (HIGH)");
  }
}

void setup() {
  Serial.begin(115200);

  // 等待 USB Serial 連線就緒 (最多等 3 秒)
  uint32_t start = millis();
  while (!Serial && (millis() - start < 3000));

  pinMode(TEST_PIN, INPUT_PULLUP);
  lastReading = stableState = digitalRead(TEST_PIN);
  changedAt = millis();

  Serial.println("\n==================================");
  Serial.println(">>> ESP32-C3 按鈕測試程式已啟動！<<<");
  Serial.println("==================================");
  Serial.println("按下或放開旋鈕時才會顯示新狀態。");
  printState(stableState);
}

void loop() {
  const bool reading = digitalRead(TEST_PIN);
  const uint32_t now = millis();

  if (reading != lastReading) {
    lastReading = reading;
    changedAt = now;
  }

  if (reading != stableState && now - changedAt >= DEBOUNCE_MS) {
    stableState = reading;
    printState(stableState);
  }

  delay(1);
}
