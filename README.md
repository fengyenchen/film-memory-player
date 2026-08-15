# Film Memory Player

以旋轉底片為互動概念的數位照片播放器。本儲存庫收錄 2 種開發板版本，它們的接腳配置與燒錄設定不同，請依實際硬體選擇。

![](film_memory_player.png)

## 硬體版本

| 版本 | 開發板 | 專案文件 |
| --- | --- | --- |
| XIAO ESP32-C3 | Seeed Studio XIAO ESP32-C3 | [進入專案](xiao-esp32c3/README.md) |
| ESP32-S3 Super Mini | 具 B+ / B- 電池焊盤的 ESP32-S3 Super Mini | [進入專案](esp32-s3-super-mini/README.md) |

每個版本都包含各自的韌體、接線說明、材料清單與 TFT 設定。外殼的 STL 檔可到 [`3d-print-parts/`](3d-print-parts/) 取得。

## 資料夾導覽

```text
.
├─ 3d-print-parts/          外殼的 3D 列印零件
├─ xiao-esp32c3/            XIAO ESP32-C3 版本
└─ esp32-s3-super-mini/     ESP32-S3 Super Mini 版本
```
