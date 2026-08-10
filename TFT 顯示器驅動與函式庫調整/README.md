# TFT 顯示器函式庫設定

## 1. 為什麼需要修改 TFT_eSPI 函式庫？

本專案使用 **1.44 吋 128×128 SPI TFT 顯示器**，搭配 **Seeed Studio XIAO ESP32-C3**。

雖然顯示器硬體規格標示的驅動晶片為 ST7735S，但實際測試後發現，使用 TFT_eSPI 函式庫並以 **ST7789 驅動模式**初始化時，顯示位置、色彩與刷新穩定度較符合目前使用的面板。

因此本專案沒有直接使用 TFT_eSPI 的預設設定，而是依照實際硬體修改函式庫的顯示器與 SPI 參數。

---

## 2. 指定 ST7789 驅動

在 TFT_eSPI 的設定檔中加入：

```cpp
#define ST7789_DRIVER
```

這代表 TFT_eSPI 在初始化螢幕時，會使用 ST7789 對應的初始化流程與 SPI 指令。

雖然本顯示器規格標示為 ST7735S，但 ST7735S 與 ST7789 同屬 Sitronix ST77xx 系列，部分基本顯示控制指令具有相似的操作方式。

本專案最後採用 ST7789 並不是認為兩顆晶片完全相同，而是依照**實際燒錄與顯示測試結果**選擇最適合目前面板的設定。

---

## 3. 設定 128×128 解析度

本專案使用的 TFT 可視區域為：

```text
128 × 128 pixels
```

因此設定：

```cpp
#define TFT_WIDTH  128
#define TFT_HEIGHT 128
```

讓 TFT_eSPI 以 128×128 作為基本顯示範圍。

這項設定會直接影響：

* 畫布大小
* 圖片繪製範圍
* 座標計算
* JPEG 顯示區域
* 畫面邊界判斷

本專案上傳的照片同樣會先在瀏覽器端裁切並縮放成 **128×128 JPEG**，因此軟體圖片尺寸與實體螢幕解析度保持一致。

---

## 4. 啟用 Display Inversion

目前使用的 TFT 面板若直接套用預設顯示設定，實際顯示效果會與預期色彩狀態不同。

因此加入：

```cpp
#define TFT_INVERSION_ON
```

強制在初始化過程中啟用 Display Inversion。

這裡的「反色」並不單純代表把：

```text
黑 → 白
白 → 黑
```

而是控制 LCD 面板內部像素電壓與顯示極性的處理方式。

不同 TFT 面板即使使用相同或相近的 Driver IC，也可能因為面板結構、廠商初始化參數不同，而需要不同的 Inversion 設定。

因此本專案使用：

```cpp
#define TFT_INVERSION_ON
```

是依照**實際面板顯示結果**決定，而不是所有 ST7789 螢幕都一定需要開啟。

---

# XIAO ESP32-C3 SPI 腳位設定

## 5. 指定硬體 SPI

設定：

```cpp
#define USE_HSPI_PORT
```

讓 TFT_eSPI 使用指定的 SPI 設定，而不是完全依賴函式庫預設的腳位配置。

由於本專案使用 **XIAO ESP32-C3**，TFT 接線如下：

```cpp
#define TFT_MOSI 10
#define TFT_SCLK  8
#define TFT_CS    5
#define TFT_DC    4
#define TFT_RST   3
```

對應實際接線為：

| TFT        | XIAO ESP32-C3 |   GPIO |
| ---------- | ------------- | -----: |
| MOSI / SDA | D10           | GPIO10 |
| SCLK / SCL | D8            |  GPIO8 |
| CS         | D3            |  GPIO5 |
| DC         | D2            |  GPIO4 |
| RST / RES  | D1            |  GPIO3 |

因此 TFT_eSPI 在編譯時就會依照這組腳位產生對應的 SPI 控制程式。

---

## 6. 為什麼要自己設定腳位？

TFT_eSPI 與一般 Arduino 顯示函式庫不同。

許多顯示器函式庫是在程式中建立物件時指定腳位，例如：

```cpp
SomeDisplay tft(CS, DC, RST);
```

但 TFT_eSPI 為了提高 ESP32 等 MCU 上的繪圖效能，大量硬體設定是在**編譯階段**決定。

因此 TFT 的：

```text
Driver
Resolution
MOSI
SCLK
CS
DC
RST
SPI Frequency
```

通常需要先寫入 TFT_eSPI 的設定檔中。

這也是為什麼本專案在第一次安裝 TFT_eSPI 後，需要先進行函式庫設定，才能正確驅動目前的 TFT。

---

# SPI 傳輸速度

## 7. 將 SPI Frequency 設為 27 MHz

設定：

```cpp
#define SPI_FREQUENCY 27000000
```

代表 TFT 的 SPI Clock 使用：

```text
27 MHz
```

理論上，提高 SPI Clock 可以提升畫面更新速度，但頻率越高，訊號完整性要求也越高。

在麵包板或杜邦線原型階段，容易受到：

* 線材長度
* 接觸品質
* EMI 雜訊
* Clock 邊緣失真

影響。

SPI 頻率過高時可能出現：

```text
花屏
圖片破碎
隨機像素
局部刷新錯誤
顯示不穩定
```

因此本專案沒有追求最高 SPI Clock，而是將頻率設定為：

```text
27 MHz
```

在更新速度與訊號穩定度之間取得平衡。

---

# 字體設定

## 8. 載入需要的字體功能

TFT_eSPI 可以透過 Compile-time Flag 決定要載入哪些字體：

```cpp
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define SMOOTH_FONT
```

本專案目前啟用：

* GLCD 基本字體
* Font 2
* Font 4
* Smooth Font 支援

只啟用實際需要的字體，可以避免不必要的 Flash 使用量。
