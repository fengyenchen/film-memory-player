# 韌體

- 開發板：ESP32-S3 Super Mini（Arduino IDE 選擇 `ESP32S3 Dev Module`）。
- 顯示器：1.44 吋 128×128 ST7735S SPI。
- 函式庫：Adafruit GFX、Adafruit ST7735 and ST7789、TJpg_Decoder。
- `partitions.csv` 是 LittleFS 自訂分割表，必須保留在 Arduino 草稿資料夾。
- GPIO1 直接控制 TFT 背光：HIGH 開、LOW 關。
- 插入 USB 時仍可播放照片、操作 Encoder 及使用 Wi-Fi 管理頁。
- 長按 Encoder 約 1.2 秒開啟主選單／返回主選單。
- 主選單包含 PHOTO、CLOCK、WI-FI、SETTINGS。
- PHOTO 模式短按切換幻燈片；SETTINGS 可調整幻燈片間隔及自動休眠。
- 長按 Encoder 約 3.5 秒進入休眠。
- 進入 Wi-Fi 管理網頁時，瀏覽器會自動同步裝置時間。
- SoftAP：`FilmMemory-Setup`，密碼：`film2026`，網址：`192.168.4.1`。
