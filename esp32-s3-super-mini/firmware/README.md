# 韌體

- 開發板：ESP32-S3 Super Mini（Arduino IDE 選擇 `ESP32S3 Dev Module`）。
- 顯示器：1.44 吋 128×128 ST7735S SPI。
- 函式庫：Adafruit GFX、Adafruit ST7735 and ST7789、TJpg_Decoder。
- `partitions.csv` 是 LittleFS 自訂分割表，必須保留在 Arduino 草稿資料夾。
- GPIO1 直接控制 TFT 背光：HIGH 開、LOW 關。
- 插入 USB 時仍可播放照片、操作 Encoder 及使用 Wi-Fi 管理頁。
- 長按 Encoder 約 3.5 秒進入休眠。
- 開機時按住 Encoder 約 1.5 秒啟動 Wi-Fi 照片管理頁面。
- SoftAP：`FilmMemory-Setup`，密碼：`film2026`，網址：`192.168.4.1`。
