# 按鈕測試

這個測試用來確認 EC11 旋鈕的按壓開關是否能被 ESP32-S3 Super Mini 正確讀取。

## 接線

| EC11 按鈕腳位 | ESP32-S3 Super Mini |
| --- | --- |
| SW | GPIO8 |
| 按鈕另一腳 | GND |

## Arduino IDE 設定

- 開發板：`ESP32S3 Dev Module`
- USB CDC On Boot：`Enabled`
- Partition Scheme：`Custom`
- Serial Monitor：`115200 baud`
- 上傳檔案：`test_button.ino`

## 測試步驟

1. 完成接線後，以 USB-C 將 ESP32-S3 Super Mini 連接電腦。
2. 開啟 `test_button.ino` 並上傳。
3. 開啟 Arduino IDE 的 Serial Monitor。
4. 不按旋鈕時，應持續看到 `未按下 (HIGH)`。
5. 按住旋鈕時，應看到 LOW 訊息。
6. 放開旋鈕後，應恢復顯示 HIGH。

## 成功判定

按下與放開時，Serial Monitor 能在 LOW 與 HIGH 之間切換，表示按壓開關、GPIO8 與 GND 接線正常。

## 常見問題

- 一直顯示 HIGH：檢查 SW 是否接到 GPIO8，以及按鈕另一腳是否接到 GND。
- 一直顯示 LOW：檢查 SW 線與 GND 是否短接，或旋鈕是否卡在按下狀態。
- Serial Monitor 沒有文字：確認傳輸速率為 115200、USB CDC On Boot 已啟用，並按一下 RESET。

> 這個程式只測試旋鈕按壓，不測試左右旋轉。
