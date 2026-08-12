/*
 * Film Memory Player
 *
 * 硬體：
 *   - ESP32-S3 Super Mini（具 B+ / B- 電池焊盤版本）
 *   - ST7789 128x128 SPI TFT
 *   - EC11 旋轉編碼器（CLK、DT、SW）
 *
 * 一般操作：
 *   - 向右旋轉：下一張照片
 *   - 向左旋轉：上一張照片
 *   - 短按旋鈕：開啟／停止自動播放
 *   - 長按約 1.5 秒後放開：開啟 Wi-Fi 照片管理頁
 *   - 持續按住約 3.5 秒：關閉背光；放開後進入低功耗休眠
 *   - 休眠時按一下旋鈕並放開：重新啟動並恢復照片播放
 *
 * Wi-Fi 照片管理：
 *   - 預設 Wi-Fi 名稱：FilmMemory-Setup
 *   - 預設 Wi-Fi 密碼：film2026
 *   - 管理網址：http://192.168.4.1
 *   - 網頁會將照片裁切、縮小成 128x128 JPEG
 *   - 新照片使用日期時間檔名，最多 50 張
 *   - 下方列表提供縮圖、單張刪除與清空相冊
 *   - 在 Wi-Fi 模式短按旋鈕：關閉 Wi-Fi，回到照片播放
 *
 * 燒錄提示：
 *   - Arduino IDE 開發板選擇 ESP32S3 Dev Module，USB CDC On Boot 選擇 Enabled
 *   - Flash Size 依模組規格選擇（常見為 4MB），Partition Scheme 選擇 Custom
 *   - 若無法連線燒錄：按住 BOOT、按一下 RESET、放開 BOOT，再上傳
 */

#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <TJpg_Decoder.h>
#include <esp_sleep.h>
#include <driver/gpio.h>

// ---------- 腳位定義 (ESP32-S3 Super Mini) ----------
constexpr int PIN_TFT_BL = 1;         // GPIO1（背光: HIGH=亮）
constexpr int PIN_TFT_RST = 2;        // GPIO2
constexpr int PIN_TFT_DC = 4;         // GPIO4
constexpr int PIN_TFT_CS = 5;         // GPIO5
constexpr int PIN_ENCODER_A = 6;      // GPIO6（EC11 CLK）
constexpr int PIN_ENCODER_B = 7;      // GPIO7（EC11 DT）
constexpr int PIN_ENCODER_SW = 8;     // GPIO8（按壓與休眠喚醒）
constexpr int PIN_TFT_SCK = 9;        // GPIO9
constexpr int PIN_TFT_MOSI = 10;      // GPIO10

constexpr uint16_t DISPLAY_W = 128;   // 128x128 解析度
constexpr uint16_t DISPLAY_H = 128;
constexpr uint8_t ENCODER_STEPS_PER_DETENT = 4;
constexpr uint32_t DEBOUNCE_MS = 30;
constexpr uint32_t WIFI_HOLD_MS = 1500;        // 長按 1.5 秒進入 Wi-Fi
constexpr uint32_t POWER_OFF_HOLD_MS = 3500;   // 持續長按 3.5 秒休眠關機
constexpr uint32_t AUTOPLAY_INTERVAL_MS = 3500;
constexpr uint32_t BUTTON_AFTER_ROTATION_GUARD_MS = 500;
constexpr uint8_t MAX_PHOTOS = 50;

const char *AP_SSID = "FilmMemory-Setup";
const char *AP_PASSWORD = "film2026";

// 網頁前端自動壓縮 128x128 正方形 HTML
const char INDEX_HTML[] PROGMEM = R"FILMHTML(
<!doctype html>
<html lang="zh-Hant">

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Film Memory</title>
    <style>
        :root {
            --bg-color: #f7f3eb;
            --card-bg: #f0ebe1;
            --text-main: #2b2621;
            --text-muted: #786f66;
            --accent-gold: #b8860b;
            --accent-border: #dcd3c5;
            --btn-pick: #2b2621;
            --btn-pick-text: #f7f3eb;
            --btn-sub: #e2dad0;
            --btn-danger: #8b3a3a;
        }

        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            background-color: var(--bg-color);
            color: var(--text-main);
            font-family: "Noto Serif TC", "Songti TC", "Georgia", serif;
            -webkit-font-smoothing: antialiased;
        }

        main {
            max-width: 620px;
            margin: 0 auto;
            padding: 40px 20px 80px;
        }

        .brand {
            font-family: system-ui, -apple-system, sans-serif;
            font-size: 0.75rem;
            letter-spacing: 0.25em;
            color: var(--accent-gold);
            font-weight: 700;
            text-transform: uppercase;
            margin-bottom: 6px;
        }

        h1 {
            font-size: 1.85rem;
            font-weight: 600;
            margin: 0 0 10px 0;
            letter-spacing: 0.05em;
        }

        p {
            line-height: 1.7;
            color: var(--text-muted);
            font-size: 0.95rem;
            margin: 0 0 20px 0;
        }

        .card {
            border: 1px solid var(--accent-border);
            border-radius: 12px;
            padding: 24px;
            margin-top: 20px;
            background: var(--card-bg);
            box-shadow: 0 4px 20px rgba(43, 38, 33, 0.03);
            /* 輕微柔和陰影 */
        }

        button,
        .pick {
            display: inline-flex;
            align-items: center;
            justify-content: center;
            border: 0;
            border-radius: 6px;
            padding: 10px 20px;
            font-family: inherit;
            font-size: 0.9rem;
            font-weight: 600;
            letter-spacing: 0.03em;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .pick {
            background: var(--btn-pick);
            color: var(--btn-pick-text);
        }

        .pick:hover {
            opacity: 0.9;
        }

        button {
            background: var(--btn-sub);
            color: var(--text-main);
        }

        button:hover:not(:disabled) {
            background: #d5ccc0;
        }

        button:disabled {
            opacity: 0.4;
            cursor: not-allowed;
        }

        .danger {
            background: var(--btn-danger);
            color: #f7f3eb;
        }

        .danger:hover {
            opacity: 0.9;
        }

        input {
            display: none;
        }

        #preview {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(80px, 1fr));
            gap: 12px;
            margin-top: 20px;
        }

        /* 拍立得照片邊框效果 */
        #preview img {
            width: 100%;
            aspect-ratio: 1;
            object-fit: cover;
            border-radius: 4px;
            background: #e0d8cc;
            padding: 4px;
            border: 1px solid #d0c5b5;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.06);
        }

        .bar {
            height: 4px;
            border-radius: 2px;
            background: var(--accent-border);
            overflow: hidden;
            margin-top: 18px;
        }

        .bar i {
            display: block;
            height: 100%;
            width: 0;
            background: var(--accent-gold);
            transition: width 0.3s ease;
        }

        .row {
            display: flex;
            gap: 12px;
            flex-wrap: wrap;
            align-items: center;
        }

        .small {
            font-size: 0.85rem;
            color: var(--text-muted);
        }

        .photo-list {
            display: grid;
            gap: 8px;
            margin-top: 16px;
        }

        .photo-item {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 12px;
            padding: 9px 10px;
            border: 1px solid var(--accent-border);
            border-radius: 6px;
            background: var(--bg-color);
        }

        .photo-info {
            display: flex;
            align-items: center;
            min-width: 0;
            gap: 10px;
        }

        .stored-thumb {
            width: 58px;
            height: 58px;
            flex: 0 0 58px;
            object-fit: cover;
            border-radius: 4px;
            border: 1px solid var(--accent-border);
            background: #e0d8cc;
        }

        .photo-name {
            font-family: monospace;
            font-size: 0.82rem;
            overflow-wrap: anywhere;
        }

        .delete-one {
            padding: 7px 12px;
            background: var(--btn-danger);
            color: #f7f3eb;
        }

        code {
            font-family: monospace;
            color: var(--accent-gold);
            background: rgba(184, 134, 11, 0.08);
            padding: 2px 6px;
            border-radius: 4px;
        }
    </style>
</head>

<body>
    <main>
        <div class="brand">FILM MEMORY</div>
        <h1>相片管理器</h1>
        <p>選擇照片後，瀏覽器將自動裁切為正方形並予以壓縮，保存至記憶相框中。</p>
        <section class="card">
            <div class="row">
                <label class="pick" for="files">選擇照片</label>
                <input id="files" type="file" accept="image/jpeg,image/png,image/webp" multiple>
                <button id="upload" disabled>裁切並上傳</button>
            </div>
            <div id="preview"></div>
            <div class="bar"><i id="progress"></i></div>
            <p id="status" class="small" style="margin-top: 12px;">等待選擇照片...</p>
        </section>
        <section class="card">
            <div class="row">
                <button id="refresh">更新列表</button>
                <button id="clear" class="danger">清空相冊</button>
            </div>
            <p id="stored" class="small" style="margin-top: 12px;">讀取中...</p>
            <div id="album" class="photo-list"></div>
        </section>
        <p class="small" style="margin-top: 28px; text-align: center;">提示：日常使用長按旋鈕可開啟此設定頁面 (IP:
            <code>192.168.4.1</code>)</p>
    </main>
    <script>
        const input = document.querySelector('#files'), preview = document.querySelector('#preview'), upload = document.querySelector('#upload');
        const statusEl = document.querySelector('#status'), progress = document.querySelector('#progress'), stored = document.querySelector('#stored');
        const album = document.querySelector('#album');
        let selected = [], storedCount = 0, albumMax = 50;

        // 使用 Canvas 在瀏覽器端將圖片裁切為正方形 JPEG
        function canvasBlob(file) {
            return new Promise((resolve, reject) => {
                const img = new Image(), url = URL.createObjectURL(file);
                img.onload = () => {
                    const s = Math.min(img.naturalWidth, img.naturalHeight),
                        sx = (img.naturalWidth - s) / 2,
                        sy = (img.naturalHeight - s) / 2,
                        c = document.createElement('canvas');
                    c.width = c.height = 128;
                    const x = c.getContext('2d');
                    x.fillStyle = '#000';
                    x.fillRect(0, 0, 128, 128);
                    x.drawImage(img, sx, sy, s, s, 0, 0, 128, 128);
                    URL.revokeObjectURL(url);
                    c.toBlob(b => b ? resolve(b) : reject(Error('圖片處理失敗')), 'image/jpeg', .82)
                };
                img.onerror = reject;
                img.src = url;
            });
        }

        // 選擇照片時觸發預覽
        input.onchange = () => {
            selected = [...input.files];
            preview.innerHTML = '';
            selected.forEach(f => {
                const i = document.createElement('img');
                i.src = URL.createObjectURL(f);
                preview.append(i);
            });
            upload.disabled = !selected.length;
            statusEl.textContent = `已選擇 ${selected.length} 張照片`;
        };

        // 使用瀏覽器的本機日期時間產生排序穩定、且不會重複使用空號的檔名。
        function photoFileName(timeMs) {
            const d = new Date(timeMs), pad = (value, length = 2) => String(value).padStart(length, '0');
            return `${d.getFullYear()}${pad(d.getMonth() + 1)}${pad(d.getDate())}_${pad(d.getHours())}${pad(d.getMinutes())}${pad(d.getSeconds())}_${pad(d.getMilliseconds(), 3)}.jpg`;
        }

        // 點擊上傳按鈕：依序處理並上傳照片
        upload.onclick = async () => {
            upload.disabled = true;
            try {
                await refresh(false);
                if (storedCount + selected.length > albumMax) {
                    throw Error(`最多可存 ${albumMax} 張，目前已有 ${storedCount} 張`);
                }
                const batchStartedAt = Date.now();
                for (let n = 0; n < selected.length; n++) {
                    statusEl.textContent = `處理並上傳中 ${n + 1} / ${selected.length}`;
                    const blob = await canvasBlob(selected[n]),
                        name = photoFileName(batchStartedAt + n),
                        form = new FormData();
                    form.append('file', blob, name);
                    const r = await fetch('/upload', { method: 'POST', body: form });
                    if (!r.ok) throw Error(await r.text());
                    progress.style.width = ((n + 1) / selected.length * 100) + '%';
                }
                statusEl.textContent = '上傳完成！';
                selected = [];
                input.value = '';
                preview.innerHTML = '';
                await refresh(false);
            } catch (e) {
                statusEl.textContent = '失敗：' + e.message;
            } finally {
                upload.disabled = false;
            }
        };

        // 刷新並取得 ESP32 儲存空間狀況
        async function refresh(showStatus = false) {
            try {
                const r = await fetch('/list?t=' + Date.now(), { cache: 'no-store' });
                if (!r.ok) throw Error('讀取失敗');
                const j = await r.json();
                storedCount = j.count;
                albumMax = j.max || 50;
                stored.textContent = `已儲存 ${j.count} / ${albumMax} 張照片，使用 ${Math.round(j.used / 1024)} KB / ${Math.round(j.total / 1024)} KB`;
                album.innerHTML = '';
                if (!j.files || !j.files.length) {
                    const empty = document.createElement('span');
                    empty.className = 'small';
                    empty.textContent = '相冊目前沒有照片';
                    album.append(empty);
                } else {
                    j.files.forEach(name => {
                        const row = document.createElement('div');
                        row.className = 'photo-item';
                        const info = document.createElement('div');
                        info.className = 'photo-info';
                        const thumb = document.createElement('img');
                        thumb.className = 'stored-thumb';
                        thumb.loading = 'lazy';
                        thumb.alt = name;
                        thumb.src = '/photo?name=' + encodeURIComponent(name) + '&t=' + Date.now();
                        const label = document.createElement('span');
                        label.className = 'photo-name';
                        label.textContent = name;
                        const button = document.createElement('button');
                        button.className = 'delete-one';
                        button.textContent = '刪除';
                        button.onclick = () => deletePhoto(name, button);
                        info.append(thumb, label);
                        row.append(info, button);
                        album.append(row);
                    });
                }
                if (showStatus) statusEl.textContent = '列表已更新';
            } catch (e) {
                stored.textContent = '無法讀取照片列表：' + e.message;
            }
        }

        async function deletePhoto(name, button) {
            if (!confirm(`確定要刪除 ${name} 嗎？`)) return;
            button.disabled = true;
            try {
                const r = await fetch('/delete?name=' + encodeURIComponent(name), { method: 'POST' });
                if (!r.ok) throw Error(await r.text());
                statusEl.textContent = `已刪除 ${name}`;
                await refresh(false);
            } catch (e) {
                statusEl.textContent = '刪除失敗：' + e.message;
                button.disabled = false;
            }
        }

        document.querySelector('#refresh').onclick = () => refresh(true);
        document.querySelector('#clear').onclick = async () => {
            if (confirm('確定要清空所有照片嗎？')) {
                await fetch('/clear', { method: 'POST' });
                progress.style.width = '0';
                await refresh(true);
            }
        };
        refresh(false);
    </script>
</body>

</html>
)FILMHTML";

// Adafruit 將面板偏移函式設為 protected；用小型衍生類別提供 128x128 零偏移設定。
class FilmMemoryST7789 : public Adafruit_ST7789 {
public:
  FilmMemoryST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
      : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}

  void use128x128ZeroOffset() {
    setColRowStart(0, 0);
    setRotation(0); // 重新套用 _xstart、_ystart
  }
};

FilmMemoryST7789 tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_MOSI, PIN_TFT_SCK, PIN_TFT_RST);
WebServer server(80);

String photos[MAX_PHOTOS];
uint8_t photoCount = 0;
uint8_t currentPhoto = 0;
bool fileSystemReady = false;
bool setupMode = false;
bool webRoutesConfigured = false;
bool autoplay = false;
bool sleeping = false;
uint32_t lastSlideAt = 0;
uint32_t lastEncoderActionAt = 0;

uint8_t encoderPrevious = 0;
int8_t encoderAccumulated = 0;
bool buttonLastReading = HIGH;
bool buttonStable = HIGH;
uint32_t buttonChangedAt = 0;
uint32_t buttonPressedAt = 0;
bool wifiHoldHandled = false;
bool longPressHandled = false;

File uploadFile;
bool uploadOK = false;
String uploadError;

void setBacklight(bool on) {
  digitalWrite(PIN_TFT_BL, on ? HIGH : LOW);
}

void drawCentered(const String &text, int16_t y, uint8_t size, uint16_t color) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  tft.setCursor((DISPLAY_W - w) / 2, y);
  tft.print(text);
}

void showMessage(const String &title, const String &detail = "") {
  tft.fillScreen(ST77XX_BLACK);
  drawCentered(title, 20, 1, ST77XX_WHITE);
  if (detail.length()) drawCentered(detail, 45, 1, ST77XX_CYAN);
}

bool tftJpegOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // false 代表「停止整張 JPEG 解碼」，超出畫面的區塊只能略過並回傳 true。
  if (x >= DISPLAY_W || y >= DISPLAY_H) return true;

  const uint16_t sourceW = w;
  const uint16_t drawW = (x + w > DISPLAY_W) ? (DISPLAY_W - x) : w;
  const uint16_t drawH = (y + h > DISPLAY_H) ? (DISPLAY_H - y) : h;

  // 若右側需要裁切，逐列繪製才能保留來源 bitmap 原本的列寬。
  if (drawW == sourceW) {
    tft.drawRGBBitmap(x, y, bitmap, drawW, drawH);
  } else {
    for (uint16_t row = 0; row < drawH; ++row) {
      tft.drawRGBBitmap(x, y + row, bitmap + row * sourceW, drawW, 1);
    }
  }
  return true;
}

void drawPhoto() {
  if (setupMode || sleeping) return;
  if (!fileSystemReady || photoCount == 0) {
    showMessage("NO PHOTOS", "Hold 1.5s for WiFi");
    return;
  }
  tft.fillScreen(ST77XX_BLACK);
  TJpgDec.drawFsJpg(0, 0, photos[currentPhoto].c_str(), LittleFS);
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

void handleList() {
  scanPhotos();
  String files = "[";
  for (uint8_t index = 0; index < photoCount; ++index) {
    if (index > 0) files += ',';
    String name = photos[index];
    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    files += "\"" + name + "\"";
  }
  files += "]";
  String json = "{\"count\":" + String(photoCount) +
                ",\"used\":" + String(LittleFS.usedBytes()) +
                ",\"total\":" + String(LittleFS.totalBytes()) +
                ",\"max\":" + String(MAX_PHOTOS) +
                ",\"files\":" + files + "}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", json);
}

bool findPhotoPath(const String &name, String &path) {
  if (name.isEmpty() || name.length() > 48 || name.indexOf('/') >= 0 ||
      name.indexOf('\\') >= 0 || name.indexOf("..") >= 0) {
    return false;
  }
  String lower = name;
  lower.toLowerCase();
  if (!lower.endsWith(".jpg") && !lower.endsWith(".jpeg")) return false;
  path = "/photos/" + name;
  return LittleFS.exists(path.c_str());
}

void handlePhoto() {
  if (!server.hasArg("name")) {
    server.send(400, "text/plain", "Missing filename");
    return;
  }
  String path;
  if (!findPhotoPath(server.arg("name"), path)) {
    server.send(404, "text/plain", "Photo not found");
    return;
  }
  File photo = LittleFS.open(path, FILE_READ);
  if (!photo) {
    server.send(500, "text/plain", "Cannot open photo");
    return;
  }
  server.sendHeader("Cache-Control", "no-store");
  server.streamFile(photo, "image/jpeg");
  photo.close();
}

void handleDelete() {
  if (!server.hasArg("name")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Missing filename\"}");
    return;
  }

  String path;
  if (!findPhotoPath(server.arg("name"), path)) {
    server.send(404, "application/json", "{\"ok\":false,\"error\":\"Photo not found\"}");
    return;
  }
  if (!LittleFS.remove(path.c_str())) {
    server.send(500, "application/json", "{\"ok\":false,\"error\":\"Delete failed\"}");
    return;
  }

  scanPhotos();
  server.send(200, "application/json", "{\"ok\":true,\"count\":" + String(photoCount) + "}");
}

void handleClear() {
  LittleFS.format();
  LittleFS.mkdir("/photos");
  scanPhotos();
  server.send(200, "application/json", "{\"ok\":true}");
}

void handleUploadBody() {
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    uploadOK = false;
    uploadError = "";
    scanPhotos();
    if (photoCount >= MAX_PHOTOS) {
      uploadError = "Album is full";
      return;
    }

    String name = upload.filename;
    bool valid = name.length() == 23 && name[8] == '_' && name[15] == '_' && name.endsWith(".jpg");
    for (uint8_t index = 0; valid && index < 19; ++index) {
      if (index == 8 || index == 15) continue;
      valid = name[index] >= '0' && name[index] <= '9';
    }
    if (!valid) {
      uploadError = "Invalid photo filename";
      return;
    }
    String path = "/photos/" + name;
    if (LittleFS.exists(path.c_str())) {
      uploadError = "Photo filename already exists";
      return;
    }

    uploadFile = LittleFS.open(path, FILE_WRITE);
    uploadOK = static_cast<bool>(uploadFile);
    if (!uploadOK) uploadError = "Cannot create photo file";
  } else if (upload.status == UPLOAD_FILE_WRITE && uploadOK) {
    if (uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize) {
      uploadOK = false;
      uploadError = "Storage write failed";
    }
  } else if (upload.status == UPLOAD_FILE_END && uploadOK) {
    if (uploadFile) uploadFile.close();
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    if (uploadFile) uploadFile.close();
    uploadOK = false;
    uploadError = "Upload aborted";
  }
}

void handleUploadDone() {
  if (uploadFile) uploadFile.close();
  if (!uploadOK) {
    server.send(507, "application/json", "{\"ok\":false,\"error\":\"" + uploadError + "\"}");
    return;
  }
  scanPhotos();
  server.send(200, "application/json", "{\"ok\":true,\"count\":" + String(photoCount) + "}");
}

void startSetupServer() {
  setupMode = true;
  
  // 先關閉再開啟，確保 Wi-Fi 晶片乾淨重啟
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_AP);
  
  // 強制設定 Channel 1，避免頻段相衝
  bool success = WiFi.softAP(AP_SSID, AP_PASSWORD, 1, 0, 4);
  
  if (success) {
    showMessage("WIFI READY", "192.168.4.1");
    Serial.println("Wi-Fi 熱點開啟成功！");
  } else {
    showMessage("WIFI ERROR", "Rebooting...");
    delay(2000);
    ESP.restart(); // 若失敗自動重啟
  }

  if (!webRoutesConfigured) {
    server.on("/list", HTTP_GET, handleList);
    server.on("/photo", HTTP_GET, handlePhoto);
    server.on("/delete", HTTP_POST, handleDelete);
    server.on("/clear", HTTP_POST, handleClear);
    server.on("/upload", HTTP_POST, handleUploadDone, handleUploadBody);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Cache-Control", "no-store");
      server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
    });
    webRoutesConfigured = true;
  }
  server.begin();
}

void changePhoto(int direction) {
  if (photoCount == 0) return;
  int next = int(currentPhoto) + direction;
  if (next < 0) next = photoCount - 1;
  if (next >= photoCount) next = 0;
  currentPhoto = next;
  autoplay = false;
  lastEncoderActionAt = millis();
  lastSlideAt = millis();
  drawPhoto();
}

void enterSleep() {
  autoplay = false;
  sleeping = true;
  if (setupMode) {
    server.stop();
    setupMode = false;
  }
  WiFi.mode(WIFI_OFF);
  tft.fillScreen(ST77XX_BLACK);
  setBacklight(false);
  // 鎖住背光 GPIO 的 LOW，避免進入 Light-sleep 後腳位浮動、背光自行亮起。
  gpio_hold_en(static_cast<gpio_num_t>(PIN_TFT_BL));
  delay(80);

  // 休眠門檻是在按鍵仍為 LOW 時觸發。等本次長按放開並穩定為 HIGH，
  // 才允許下一次新的 LOW 作為喚醒事件。
  while (digitalRead(PIN_ENCODER_SW) == LOW) delay(10);
  uint32_t releasedAt = millis();
  while (millis() - releasedAt < 200) {
    if (digitalRead(PIN_ENCODER_SW) == LOW) releasedAt = millis();
    delay(5);
  }

  // 清除可能殘留的喚醒來源／GPIO 狀態，再設定 Encoder SW 低電位喚醒。
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  gpio_wakeup_disable(static_cast<gpio_num_t>(PIN_ENCODER_SW));
  const esp_err_t gpioResult =
      gpio_wakeup_enable(static_cast<gpio_num_t>(PIN_ENCODER_SW), GPIO_INTR_LOW_LEVEL);
  const esp_err_t sourceResult = esp_sleep_enable_gpio_wakeup();
  Serial.printf("Sleep setup: gpio=%d source=%d\n", gpioResult, sourceResult);
  Serial.flush();

  esp_err_t sleepResult = ESP_FAIL;
  uint32_t sleepStartedAt = millis();
  if (gpioResult == ESP_OK && sourceResult == ESP_OK) {
    sleepResult = esp_light_sleep_start();
    Serial.printf("Light-sleep returned: %d, cause=%d\n", sleepResult,
                  esp_sleep_get_wakeup_cause());
  }

  gpio_wakeup_disable(static_cast<gpio_num_t>(PIN_ENCODER_SW));
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  const uint32_t sleptForMs = millis() - sleepStartedAt;

  // 若核心拒絕睡眠或被殘留狀態提前喚醒，仍保持背光關閉，
  // 等待使用者下一次明確按下，避免裝置自行變亮。
  if (sleepResult != ESP_OK ||
      esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_GPIO ||
      sleptForMs < 300) {
    while (digitalRead(PIN_ENCODER_SW) == HIGH) delay(10);
  }
  while (digitalRead(PIN_ENCODER_SW) == LOW) delay(10);

  // Light-sleep 後重新啟動，確保 TFT、LittleFS、Encoder 全部重新初始化。
  gpio_hold_dis(static_cast<gpio_num_t>(PIN_TFT_BL));
  digitalWrite(PIN_TFT_BL, LOW);
  ESP.restart();
}

void pollEncoder() {
  const uint8_t state = (digitalRead(PIN_ENCODER_A) << 1) | digitalRead(PIN_ENCODER_B);
  if (state != encoderPrevious) lastEncoderActionAt = millis();
  const uint8_t transition = (encoderPrevious << 2) | state;
  static const int8_t table[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
  encoderAccumulated += table[transition & 0x0F];
  encoderPrevious = state;

  if (encoderAccumulated >= ENCODER_STEPS_PER_DETENT) {
    encoderAccumulated = 0; changePhoto(1);
  } else if (encoderAccumulated <= -ENCODER_STEPS_PER_DETENT) {
    encoderAccumulated = 0; changePhoto(-1);
  }
}

void pollButton() {
  const bool reading = digitalRead(PIN_ENCODER_SW);
  uint32_t now = millis();
  if (reading != buttonLastReading) buttonChangedAt = now;

  if (now - buttonChangedAt > DEBOUNCE_MS && reading != buttonStable) {
    buttonStable = reading;
    if (buttonStable == LOW) {
      buttonPressedAt = now;
      wifiHoldHandled = false;
      longPressHandled = false;
    } else if (!wifiHoldHandled && !longPressHandled) {
      // 短按切換：如果在 Wi-Fi 模式，短按會退出並進入相片播放
      if (setupMode) {
        setupMode = false;
        server.stop();
        WiFi.mode(WIFI_OFF);
        scanPhotos();
        drawPhoto();
      } else {
        if (now - lastEncoderActionAt >= BUTTON_AFTER_ROTATION_GUARD_MS) {
          autoplay = !autoplay;
          lastSlideAt = millis();
        } else {
          Serial.println("Short press ignored after encoder rotation");
        }
      }
    }
  }

  // 正常播放時直接長按 1.5 秒開啟 Wi-Fi 管理頁。
  if (buttonStable == LOW && !wifiHoldHandled &&
      (now - buttonPressedAt >= WIFI_HOLD_MS)) {
    wifiHoldHandled = true;
    if (!setupMode) startSetupServer();
  }

  // 繼續按住到 3.5 秒才休眠關機（即使已進入 Wi-Fi 也有效）。
  if (buttonStable == LOW && !longPressHandled &&
      (now - buttonPressedAt >= POWER_OFF_HOLD_MS)) {
    longPressHandled = true;
    enterSleep();
  }
  buttonLastReading = reading;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // 若前一次休眠鎖住背光腳，開機時先解除，再由程式正常控制。
  gpio_hold_dis(static_cast<gpio_num_t>(PIN_TFT_BL));
  pinMode(PIN_TFT_BL, OUTPUT);
  setBacklight(false); // 初始化完成前先關背光，避免看到白屏／雜訊

  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  pinMode(PIN_ENCODER_SW, INPUT_PULLUP); // 啟用 GPIO8 內部上拉電阻

  tft.init(DISPLAY_W, DISPLAY_H);
  // 這塊 128x128 ST7789 的可視記憶體從 (0,0) 開始。
  // Adafruit 對其他尺寸會自動置中，必須覆寫成與 test.ino 相同的 0~127。
  tft.use128x128ZeroOffset();
  tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);
  tft.sendCommand(0x36, (const uint8_t[]){0x08}, 1); // BGR 色彩修正
  setBacklight(true);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(false);
  TJpgDec.setCallback(tftJpegOutput);

  encoderPrevious = (digitalRead(PIN_ENCODER_A) << 1) | digitalRead(PIN_ENCODER_B);
  buttonLastReading = buttonStable = digitalRead(PIN_ENCODER_SW);
  if (buttonStable == LOW) buttonPressedAt = millis();

  fileSystemReady = LittleFS.begin(true);
  if (!fileSystemReady) {
    showMessage("FS ERROR", "Formatting...");
    LittleFS.format();
    LittleFS.begin(true);
  }
  LittleFS.mkdir("/photos");
  scanPhotos();

  drawPhoto();
}

void loop() {
  pollButton();

  if (setupMode) {
    server.handleClient();
    delay(2);
    return;
  }

  pollEncoder();
  if (autoplay && photoCount > 1 && millis() - lastSlideAt >= AUTOPLAY_INTERVAL_MS) {
    currentPhoto = (currentPhoto + 1) % photoCount;
    lastSlideAt = millis();
    drawPhoto();
  }
  delay(1);
}
