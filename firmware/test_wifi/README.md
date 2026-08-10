# Wi-Fi 熱點測試

這個測試用來確認 XIAO ESP32-C3 能否建立 Wi-Fi SoftAP 熱點，以及手機能否找到並連線。

## 測試前準備

- 將外接 Wi-Fi 天線垂直扣緊在 XIAO 的天線接座。
- 天線不要貼在電池、TFT 金屬背板或大量導線旁邊。
- 這個測試不需要連接 TFT 或 EC11。

## Arduino IDE 設定

- 開發板：`XIAO_ESP32C3`
- Serial Monitor：`115200 baud`
- 上傳檔案：`test_wifi.ino`

## 測試步驟

1. 使用 USB-C 將 XIAO 連接電腦。
2. 上傳 `test_wifi.ino`。
3. 開啟 Serial Monitor。
4. 用手機搜尋 Wi-Fi 網路 `XIAO-Test-AP`。
5. 這是測試用的開放網路，不需要密碼。
6. 連線後，Serial Monitor 應顯示熱點 IP，通常是 `192.168.4.1`。

## 成功判定

- Serial Monitor 顯示 `熱點發射成功`。
- 手機可以找到並連上 `XIAO-Test-AP`。
- 手機提示「沒有網際網路」是正常現象，這個熱點只用於裝置本機通訊。

## 常見問題

- 手機找不到熱點：確認天線接頭已扣緊，然後按一下 XIAO Reset。
- Serial Monitor 沒有文字：確認傳輸速率為 115200。
- 顯示熱點發射失敗：重新插拔 USB-C，再次上傳測試程式。
- 手機自動切回行動網路：暫時關閉手機的自動切換網路功能，再重新連線。

> 正式韌體使用的熱點名稱與密碼和此測試程式不同；這個開放熱點只用於確認 Wi-Fi 硬體功能。
