#include <Arduino.h>

// 腳位定義
constexpr int PIN_BL = 1;
#define PIN_RST  2  // GPIO2
#define PIN_DC   4  // GPIO4
#define PIN_CS   5  // GPIO5
#define PIN_SCL  9  // GPIO9
#define PIN_SDA 10  // GPIO10

void writeBit(bool bit) {
  digitalWrite(PIN_SDA, bit);
  digitalWrite(PIN_SCL, LOW);
  delayMicroseconds(3); // 稍微拉長時間確保穩定
  digitalWrite(PIN_SCL, HIGH);
  delayMicroseconds(3);
}

void writeCmd(uint8_t cmd) {
  digitalWrite(PIN_DC, LOW);
  digitalWrite(PIN_CS, LOW);
  for (int i = 7; i >= 0; i--) writeBit((cmd >> i) & 1);
  digitalWrite(PIN_CS, HIGH);
}

void writeData(uint8_t data) {
  digitalWrite(PIN_DC, HIGH);
  digitalWrite(PIN_CS, LOW);
  for (int i = 7; i >= 0; i--) writeBit((data >> i) & 1);
  digitalWrite(PIN_CS, HIGH);
}

// 設定 128x128 繪製視窗邊界
void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  writeCmd(0x2A); // CASET
  writeData(x0 >> 8); writeData(x0 & 0xFF);
  writeData(x1 >> 8); writeData(x1 & 0xFF);

  writeCmd(0x2B); // RASET
  writeData(y0 >> 8); writeData(y0 & 0xFF);
  writeData(y1 >> 8); writeData(y1 & 0xFF);
}

void setup() {
  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, LOW); // 初始化完成前先關閉背光，避免白屏或雜色

  pinMode(PIN_RST, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_SCL, OUTPUT);
  pinMode(PIN_SDA, OUTPUT);

  digitalWrite(PIN_CS, HIGH);

  // 硬體重設 (RST 脈衝)
  digitalWrite(PIN_RST, HIGH); delay(10);
  digitalWrite(PIN_RST, LOW);  delay(50);
  digitalWrite(PIN_RST, HIGH); delay(120);

  // ST7789 初始化指令序列
  writeCmd(0x01); delay(150); // SWRESET
  writeCmd(0x11); delay(120); // SLPOUT
  writeCmd(0x3A); writeData(0x55); // COLMOD (16-bit RGB565)

  // 設定 MADCTL (0x36) 強制使用 RGB 通道（避免紅藍反轉）
  writeCmd(0x36); writeData(0x08); // 0x00: RGB, 0x08: BGR
  writeCmd(0x20); // 0x21 開啟反色；0x20 關閉反色
  writeCmd(0x29); delay(50);  // DISPON

  // 1. 強制設定繪製視窗範圍為 0~127 (共 128x128 像素)
  setWindow(0, 0, 127, 127);

  // 2. 灌入純紅色像素 (0xF800)
  writeCmd(0x2C); // RAMWR
  for (int32_t i = 0; i < 128 * 128; i++) {
    writeData(0xF8); // 高位元
    writeData(0x00); // 低位元
  }

  // TFT BL 直接接 GPIO1；畫面寫完後才開啟背光。
  digitalWrite(PIN_BL, HIGH);
}

void loop() {
}
