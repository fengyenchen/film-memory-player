# Film Memory Player 共用韌體

這是共用的正式韌體。程式會依 Arduino IDE 選擇的編譯目標，自動在 `Config.h` 套用對應 GPIO。使用 128×128 SPI TFT、EC11 旋轉編碼器與板載 LittleFS，提供照片播放、時鐘、表情、Wi-Fi 相簿管理、設定與休眠功能。

## 功能

- 從 LittleFS 播放最多 50 張照片。
- 手動切換照片或啟用自動播放。
- 顯示日期、星期與時間。
- 旋轉選擇 10 種黑底白線表情。
- 透過手機瀏覽器上傳、預覽、刪除或清空照片。
- 調整幻燈片間隔、自動休眠時間與螢幕亮度。
- 長按旋鈕進入低功耗休眠，再按一下喚醒。
- 自動保存設定與最後選擇的表情。

## Arduino IDE 準備

### 1. 安裝開發板套件

在 Arduino IDE 的 Boards Manager 安裝 Espressif 的 `esp32` 套件，再依開發板設定：

| 選項 | ESP32-S3 Super Mini | XIAO ESP32-C3 |
| --- | --- | --- |
| Board | `ESP32S3 Dev Module` | `ESP32C3 Dev Module` |
| USB CDC On Boot | `Enabled` | `Enabled` |
| Partition Scheme | `Custom` | `Custom` |
| Serial Monitor | `115200 baud` | `115200 baud` |

### 2. 安裝函式庫

從 Library Manager 安裝：

- `Adafruit GFX Library`
- `Adafruit ST7735 and ST7789 Library`
- `TJpg_Decoder`

`LittleFS`、`WiFi`、`WebServer`、`Preferences` 與 `SPI` 由 ESP32 開發板套件提供。

## 編譯與燒錄

1. 保持這個資料夾名稱為 `FilmMemoryPlayer`。
2. 確認所有 `.cpp`、`.h`、`.ino` 與 `partitions.csv` 都在同一個資料夾。
3. 使用 Arduino IDE 開啟 [FilmMemoryPlayer.ino](FilmMemoryPlayer.ino)。
4. 依實際硬體選擇開發版，程式會自動套用對應接腳。
5. 選擇正確的 USB 連接埠。
6. 按下 Verify 確認可以編譯。
7. 按下 Upload 燒錄。
8. 上傳完成後按一下開發板的 RESET。

若無法進入燒錄模式：

1. 按住開發板的 BOOT。
2. 按一下 RESET。
3. 放開 BOOT。
4. 再次執行 Upload。

`partitions.csv` 會建立約 1.94 MiB 的照片儲存空間。請勿刪除或改名；更換 Partition Scheme、格式化或重新分割 Flash 可能清除已上傳的照片。

## 首次使用

1. 開機後裝置進入 `PHOTO` 模式。
2. 如果尚未上傳照片，畫面會顯示 `NO PHOTOS`。
3. 長按旋鈕約 1.2 秒後放開，進入主選單。
4. 旋轉到 `WI-FI`，短按進入。
5. 用手機連接：
   - Wi-Fi：`FilmMemory-Setup`
   - 密碼：`film2026`
6. 瀏覽器開啟 [http://192.168.4.1](http://192.168.4.1)。
7. 選取照片、裁切並上傳。
8. 在 Wi-Fi 模式短按旋鈕，關閉熱點並回到主選單。
9. 進入 `PHOTO` 開始播放。

網頁會將圖片裁切並縮放為 128×128 JPEG。管理頁開啟時也會用手機／電腦時間同步裝置時鐘。

## 操作方式

### 共通操作

| 操作 | 功能 |
| --- | --- |
| 長按約 1.2 秒後放開 | 從目前模式進入主選單 |
| 在主選單旋轉 | 選擇模式 |
| 在主選單短按 | 進入選取的模式 |
| 持續按住約 3.5 秒 | 關閉背光並進入休眠 |
| 休眠時按一下並放開 | 喚醒並重新啟動 |

### 模式

| 模式 | 旋轉 | 短按 | 說明 |
| --- | --- | --- | --- |
| `PHOTO` | 上一張／下一張 | 開啟或停止幻燈片 | 播放 LittleFS 相簿 |
| `CLOCK` | 無 | 無 | 顯示時間、日期與星期 |
| `EMOTES` | 切換表情 | 無 | 顯示 10 種靜態線條表情 |
| `WI-FI` | 無 | 關閉 Wi-Fi 並回主選單 | 啟動照片管理頁 |
| `SETTINGS` | 修改目前選項 | 切換下一個選項 | 設定幻燈片、自動休眠與亮度 |

幻燈片可選 `2`、`3.5`、`5`、`10` 秒；自動休眠可選 `OFF`、`1`、`3`、`5` 分鐘；螢幕亮度可選 `25%`、`50%`、`75%`、`100%`。設定會自動保存，重新開機後仍會套用。

如果剛旋轉完照片，程式會短暫忽略按鍵，以免轉動旋鈕時誤觸自動播放。

## Wi-Fi 相簿管理

管理頁提供：

- 多張照片依序裁切與上傳。
- 顯示剩餘儲存空間。
- 照片縮圖與檔案資訊。
- 單張刪除或清空全部照片。
- 裝置時間同步。

Wi-Fi 模式是本機 SoftAP，不需要網際網路。手機若提示「此網路無網際網路」，仍可保持連線並開啟 `192.168.4.1`。

## 資料夾結構

```text
FilmMemoryPlayer/
├─ FilmMemoryPlayer.ino   # setup()、loop() 與初始化流程
├─ Config.h               # 自動選擇 S3/C3 GPIO，以及共用尺寸與常數
├─ AppState.h/.cpp        # 共用狀態、TFT、WebServer、Preferences
├─ DisplayUtils.h/.cpp    # 背光、置中文字與訊息畫面
├─ PhotoMode.h/.cpp       # JPEG、照片掃描、顯示與切換
├─ ClockMode.h/.cpp       # 時鐘畫面
├─ EmoteMode.h/.cpp       # 線條表情繪製
├─ MenuMode.h/.cpp        # 主選單與模式切換
├─ SettingsMode.h/.cpp    # 設定畫面與保存
├─ WifiManager.h/.cpp     # Wi-Fi、上傳、刪除與時間同步
├─ WebPage.h/.cpp         # 內嵌管理網頁
├─ Encoder.h/.cpp         # EC11 旋轉、短按與長按
├─ PowerManager.h/.cpp    # 休眠與按鍵喚醒
├─ partitions.csv         # 4 MB Flash 自訂分割表
└─ README.md
```

Arduino IDE 會自動編譯同一個 sketch 資料夾內的所有 `.cpp` 檔。請勿只複製 `.ino`，否則會出現缺少標頭或未定義函式的錯誤。
