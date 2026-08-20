# TFT 螢幕測試

這個測試會直接向 128×128 TFT 寫入純紅色畫面，用來確認螢幕供電、SPI 接線、重設腳位與顯示範圍是否正常。

## 接線

| TFT 腳位 | XIAO ESP32-C3 |
| --- | --- |
| VCC / VDD | 3V3 |
| GND | GND |
| BLK / BL | GPIO2 |
| RES / RST | GPIO20（D7） |
| DC / RS | GPIO4 |
| CS / CE | GPIO5 |
| SCL / SCK | GPIO8 |
| SDA / MOSI | GPIO10 |

## Arduino IDE 設定

- 開發板：`XIAO_ESP32C3`
- USB CDC On Boot：`Enabled`
- 上傳檔案：`test_screen.ino`
- 這個測試直接傳送顯示指令，不需要額外顯示函式庫。

## 測試步驟

1. 先拔除 USB-C，再依照接線表連接 TFT。
2. 再接上 USB-C，開啟並上傳 `test_screen.ino`。
3. 等待約 1 秒觀察螢幕。

## 成功判定

整個 128×128 顯示區域呈現均勻紅色，沒有白邊、位移、雜色或破碎區塊，即代表主要螢幕接線正常。

## 背光測試方式

程式會在初始化期間先關閉 GPIO2 背光，完成純紅色畫面寫入後再開啟，因此啟動時短暫黑屏是正常現象。

## 常見問題

- 完全不亮：先確認 GPIO2 背光控制與 3V3、GND。
- 全白：優先檢查 GPIO20、GPIO4、GPIO5、GPIO8、GPIO10。
- 有畫面但位置偏移：確認使用的是本專案測試過的 128×128 面板。
- 顏色不是紅色：可能是面板色彩順序不同，可再確認初始化設定。
