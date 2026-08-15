# 接線指南

## 主要控制板：ESP32-S3 Super Mini

本文件對應具有 `B+`、`B-` 電池焊盤的 ESP32-S3 Super Mini。請將所有模組的接地（GND）連接到開發板的 `GND`，TFT 與 EC11 使用板上的 `3V3` 供電。

> **Wi-Fi 天線注意事項：**
> 這塊板使用板載 PCB 天線。天線端不要緊貼 LiPo、TFT 金屬背板或大量導線，也不要用金屬件包覆。

---

## 電池與充電

```text
LiPo 正極 → ESP32-S3 Super Mini 的 B+
LiPo 負極 → ESP32-S3 Super Mini 的 B-

USB-C 充電器 → ESP32-S3 Super Mini 原生 USB-C
```

USB-C 線直接插入開發板，另一端接一般 5V USB 充電器。只使用單節標稱 3.7V、充滿 4.2V 且具有保護板的 LiPo。

依此版本接腳圖標註，板載充電電路的預設充電電流約為 100mA。不同賣家的板子可能使用不同充電晶片，實際規格仍以商品頁為準。

> **電池安全：**
> `B+`、`B-` 不可接反；不可將 5V 接到 `B+`；不要同時外接 TP4056 與板載充電電路替同一顆電池充電。

---

## 螢幕模組 (ST7735S 128x128 SPI TFT)

這塊螢幕使用 SPI 通訊。背光由 ESP32-S3 Super Mini 的 GPIO1 直接控制。

| TFT 螢幕腳位標示 | ESP32-S3 Super Mini | 說明 |
| --- | --- | --- |
| **VCC / VDD** | `3V3` | 螢幕電源 (3.3V) |
| **GND** | `GND` | 接地 |
| **SCL / SCK** | GPIO9 | SPI 時脈線 (Clock) |
| **SDA / MOSI** | GPIO10 | SPI 資料線 (Data) |
| **RES / RST** | GPIO2 | 螢幕重置 (Reset) |
| **DC / RS** | GPIO4 | 資料/指令切換 (Data/Command) |
| **CS / CE** | GPIO5 | 晶片選擇 (Chip Select) |
| **BLK / BL** | GPIO1 | 螢幕背光直接控制 |

---

## 背光開關

```text
ESP32-S3 Super Mini GPIO1 → TFT BLK / BL
```

> **💡 背光注意事項：**
> GPIO1 為 HIGH 時背光開啟，LOW 時背光關閉。進入休眠模式時，程式會將 GPIO1 鎖定為 LOW。

---

## 旋轉編碼器模組 (EC11)

EC11 旋轉編碼器通常有 5 支腳，一側 3 支（旋轉訊號），另一側 2 支（按鈕訊號）。

| EC11 腳位標示 | ESP32-S3 Super Mini | 說明 |
| --- | --- | --- |
| **GND / 共用端 C** | `GND` | 旋轉訊號的共用接地 |
| **+ / VCC** | `3V3` | 供電 |
| **CLK / A 相** | GPIO6 | 旋轉觸發訊號 A |
| **DT / B 相** | GPIO7 | 旋轉觸發訊號 B |
| **SW / 按壓開關** | GPIO8 | 按下按鈕及休眠喚醒訊號 |

> **💡 編碼器注意事項：**
> 1. GPIO6、GPIO7、GPIO8 直接依照上表接線即可。
> 2. 如果發現「向右轉」卻變成「上一張照片」，只要將 **CLK（GPIO6）** 與 **DT（GPIO7）** 兩條線互換。
