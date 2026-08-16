# Film Memory Player

Film Memory Player 是一個以「旋轉底片」為互動概念的數位照片播放器。使用 Seeed Studio XIAO ESP32-C3、128×128 SPI TFT 與 EC11 旋轉編碼器，完成一台能播放照片、切換自動播放並透過 Wi-Fi 管理相冊的小型裝置。USB-C 可同時供電、燒錄與替板載 LiPo 充電

## 完成後可以做什麼

- 左右旋轉 EC11 切換上一張或下一張照片。
- 短按旋鈕開啟或停止自動播放。
- 長按約 1.2 秒開啟主選單或返回主選單。
- 主選單可選擇相片、時鐘、表情、Wi-Fi 管理與設定。
- 從手機或電腦一次選擇多張照片上傳。
- 網頁會將照片裁切並縮放成 128×128 JPEG。
- 相片列表提供縮圖、日期時間檔名與單張刪除。
- 長按約 3.5 秒關閉背光並進入休眠；再按一下即可重新啟動。
- 最多儲存 50 張照片。

## 主要材料

- Seeed Studio XIAO ESP32-C3
- 128×128 SPI TFT 顯示器
- EC11 旋轉編碼器（含按壓功能）
- 具保護板的單節 3.7V LiPo
- USB-C 傳輸線
- 杜邦線或焊接線材
- 3D 列印外殼

完整品項請看 [材料清單](docs/BOM.md)，接線方式請看 [接線指南](docs/WIRING.md)。

## 資料夾導覽

```text
film_memory_player/
├─ firmware/FilmMemoryPlayer/        S3 與 C3 共用的正式韌體
└─ xiao-esp32c3/
   ├─ docs/
   │  ├─ BOM.md                      材料清單
   │  └─ WIRING.md                   接線指南
   ├─ firmware/
   │  ├─ test_button/                EC11 按壓測試
   │  ├─ test_screen/                TFT 紅色畫面測試
   │  └─ test_wifi/                  Wi-Fi 熱點測試
```

外殼的 STL 檔可到專案最外層的 [`3d-print-parts/`](../3d-print-parts/) 取得。

## 建議流程

### 1. 安裝 Arduino IDE 與開發板

在 Arduino IDE 中安裝 ESP32 開發板支援，並選擇：

- 開發板：`ESP32C3 Dev Module`
- USB CDC On Boot：`Enabled`
- Flash Size：`4MB`
- Partition Scheme：`Custom`
- Serial Monitor：`115200 baud`

### 2. 安裝正式韌體需要的函式庫

從 Arduino IDE Library Manager 安裝：

- `Adafruit GFX Library`
- `Adafruit ST7735 and ST7789 Library`
- `TJpg_Decoder`

`LittleFS`、`WiFi` 與 `WebServer` 由 ESP32 開發板套件提供。

### 3. 依序測試元件

建議先完成單項測試，再燒錄正式韌體：

1. [按鈕測試](firmware/test_button/README.md)：確認 GPIO21 能讀到按下與放開。
2. [螢幕測試](firmware/test_screen/README.md)：確認 128×128 顯示區域能完整顯示紅色。
3. [Wi-Fi 測試](firmware/test_wifi/README.md)：確認手機能找到並連上測試熱點。

每個測試資料夾都有獨立的 `.ino` 與操作說明。測試程式會取代板上原本的程式，完成測試後再上傳正式韌體即可。

### 4. 燒錄正式韌體

1. 開啟專案最外層的 [`firmware/FilmMemoryPlayer/FilmMemoryPlayer.ino`](../firmware/FilmMemoryPlayer/FilmMemoryPlayer.ino)。
2. 確認同一個資料夾內保留 `partitions.csv`。
3. 編譯並上傳。
4. 上傳完成後按一下 XIAO Reset。

如果 Arduino IDE 無法連線：

1. 按住 XIAO 的 BOOT。
2. 按一下 RESET。
3. 放開 BOOT。
4. 再次執行上傳。

## 接線摘要

| 功能 | XIAO GPIO |
| --- | ---: |
| TFT 背光 BL | 2 |
| TFT RST | 3 |
| TFT DC | 4 |
| TFT CS | 5 |
| EC11 CLK | 6 |
| EC11 DT | 7 |
| TFT SCL | 8 |
| TFT SDA | 10 |
| EC11 SW | 21 |

TFT VCC 接 `3V3`，TFT 與 EC11 的 GND 都接到 XIAO GND。詳細接法以 [WIRING.md](docs/WIRING.md) 為準。

## 操作方式

| 操作 | 功能 |
| --- | --- |
| 向右旋轉 | 下一張照片 |
| 向左旋轉 | 上一張照片 |
| 短按 | 開啟／停止自動播放 |
| 長按約 1.2 秒後放開 | 開啟主選單／返回主選單 |
| 持續按住約 3.5 秒 | 關閉背光並進入休眠 |
| 休眠後按一下 | 重新啟動 |

## 上傳與管理照片

1. 長按旋鈕約 1.2 秒後放開，進入主選單。
2. 旋轉到 `WI-FI`，短按進入並等待畫面顯示 `WIFI READY`。
3. 手機或電腦連接 Wi-Fi：
   - 預設名稱：`FilmMemory-Setup`
   - 預設密碼：`film2026`
4. 瀏覽器開啟 `http://192.168.4.1`。
5. 選擇照片並按下「裁切並上傳」。
6. 在下方列表查看縮圖，或刪除指定照片。
7. 在 Wi-Fi 模式短按旋鈕即可關閉 Wi-Fi 並回到主選單。

手機顯示「此網路沒有網際網路」是正常的，請保持連線並直接開啟管理網址。
