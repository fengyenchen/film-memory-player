// ============================================================================
// 1. 驅動晶片設定
// ============================================================================
#define ST7789_DRIVER      // 指定 ST7789

// 128x128 解析度
#define TFT_WIDTH  128
#define TFT_HEIGHT 128

// 強制啟用反色
#define TFT_INVERSION_ON   

// ============================================================================
// 2. ESP32-S3 Super Mini 腳位對應設定
// ============================================================================
#define USE_HSPI_PORT      // 強制 ESP32-S3 使用自訂硬體 SPI 腳位

#define TFT_MOSI 10        // GPIO10 -> 螢幕 SDA / MOSI
#define TFT_SCLK  9        // GPIO9  -> 螢幕 SCL / SCK
#define TFT_CS    5        // GPIO5  -> 螢幕 CS
#define TFT_DC    4        // GPIO4  -> 螢幕 DC
#define TFT_RST   2        // GPIO2  -> 螢幕 RES / RST

// ============================================================================
// 3. 字體與 SPI 速度設定
// ============================================================================
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define SMOOTH_FONT

// 降低一點頻率到 27MHz，避免杜邦線不穩花屏
#define SPI_FREQUENCY  27000000