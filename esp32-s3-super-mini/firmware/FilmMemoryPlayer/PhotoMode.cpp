#include "PhotoMode.h"

#include <LittleFS.h>
#include <TJpg_Decoder.h>

#include "AppState.h"
#include "DisplayUtils.h"

bool tftJpegOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // false 代表「停止整張 JPEG 解碼」，超出畫面的區塊只能略過並回傳 true。
  if (x >= DISPLAY_W || y >= DISPLAY_H) return true;

  const uint16_t sourceW = w;
  const uint16_t drawW = (x + w > DISPLAY_W) ? (DISPLAY_W - x) : w;
  const uint16_t drawH = (y + h > DISPLAY_H) ? (DISPLAY_H - y) : h;

  // TJpg_Decoder 會分區塊呼叫此函式；只複製到離屏畫布，不直接更新 TFT。
  for (uint16_t row = 0; row < drawH; ++row) {
    memcpy(photoFrame + (y + row) * DISPLAY_W + x,
           bitmap + row * sourceW,
           drawW * sizeof(uint16_t));
  }
  return true;
}

void drawPhoto() {
  if (setupMode || sleeping) return;
  if (!fileSystemReady || photoCount == 0) {
    showMessage("NO PHOTOS", "HOLD FOR MENU");
    return;
  }
  memset(photoFrame, 0, sizeof(photoFrame));
  const JRESULT decodeResult =
      TJpgDec.drawFsJpg(0, 0, photos[currentPhoto].c_str(), LittleFS);
  if (decodeResult != JDR_OK) {
    showMessage("PHOTO ERROR", "JPEG " + String(static_cast<int>(decodeResult)));
    return;
  }

  // 單一 SPI transaction 推送完整畫面；使用者只會看到完成後的照片。
  tft.drawRGBBitmap(0, 0, photoFrame, DISPLAY_W, DISPLAY_H);
}

void scanPhotos() {
  photoCount = 0;
  if (!fileSystemReady || !LittleFS.exists("/photos")) return;
  File directory = LittleFS.open("/photos");
  if (!directory || !directory.isDirectory()) return;

  File entry = directory.openNextFile();
  while (entry && photoCount < MAX_PHOTOS) {
    String path = entry.name();
    String lower = path; lower.toLowerCase();
    if (!entry.isDirectory() && (lower.endsWith(".jpg") || lower.endsWith(".jpeg"))) {
      if (!path.startsWith("/")) path = "/photos/" + path;
      photos[photoCount++] = path;
    }
    entry.close();
    entry = directory.openNextFile();
  }
  directory.close();
  // LittleFS 的列舉順序不固定；檔名排序可同時支援舊編號與日期時間檔名。
  for (uint8_t i = 0; i < photoCount; ++i) {
    for (uint8_t j = i + 1; j < photoCount; ++j) {
      if (photos[j] < photos[i]) {
        String temp = photos[i];
        photos[i] = photos[j];
        photos[j] = temp;
      }
    }
  }
  if (currentPhoto >= photoCount) currentPhoto = 0;
}

void changePhoto(int direction) {
  if (appMode != AppMode::PHOTO || photoCount == 0) return;
  int next = int(currentPhoto) + direction;
  if (next < 0) next = photoCount - 1;
  if (next >= photoCount) next = 0;
  currentPhoto = next;
  autoplay = false;
  lastEncoderActionAt = millis();
  lastSlideAt = millis();
  drawPhoto();
}
