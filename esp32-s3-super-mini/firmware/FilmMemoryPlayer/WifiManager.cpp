#include "WifiManager.h"

#include <LittleFS.h>
#include <WiFi.h>
#include <sys/time.h>
#include <time.h>

#include "AppState.h"
#include "Config.h"
#include "DisplayUtils.h"
#include "PhotoMode.h"
#include "WebPage.h"

void stopSetupServer() {
  if (!setupMode) return;
  server.stop();
  WiFi.mode(WIFI_OFF);
  setupMode = false;
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

void handleTimeSync() {
  if (!server.hasArg("epoch")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Missing epoch\"}");
    return;
  }
  const time_t epoch = static_cast<time_t>(strtoll(server.arg("epoch").c_str(), nullptr, 10));
  if (epoch < 1700000000) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"Invalid time\"}");
    return;
  }
  timeval value = {epoch, 0};
  settimeofday(&value, nullptr);
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
  appMode = AppMode::WIFI;
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
    server.on("/time", HTTP_POST, handleTimeSync);
    server.on("/upload", HTTP_POST, handleUploadDone, handleUploadBody);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Cache-Control", "no-store");
      server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
    });
    webRoutesConfigured = true;
  }
  server.begin();
}
