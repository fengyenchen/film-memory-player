#include <WiFi.h>

constexpr char AP_SSID[] = "XIAO-Test-AP";
uint8_t previousStationCount = 0;

void setup() {
  Serial.begin(115200);
  delay(2000); // 給 Serial Monitor 一點時間準備
  
  Serial.println("\n===========================");
  Serial.println("啟動 Wi-Fi AP 測試...");
  Serial.println("===========================");

  // 1. 強制清除舊的 Wi-Fi 設定並重啟 RF 模組
  WiFi.disconnect(true, true);
  delay(100);

  // 2. 設定為純基地台模式
  WiFi.mode(WIFI_AP);

  // 3. 發射「無密碼」的開放網路，排除加密長度造成的錯誤
  // (頻道設為 6，避免 1 頻道干擾)
  bool success = WiFi.softAP(AP_SSID, nullptr, 6, false, 4);

  if (success) {
    Serial.println("熱點發射成功！");
    Serial.println("請打開手機 Wi-Fi，尋找名稱為「XIAO-Test-AP」的網路（無密碼）");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("熱點發射失敗，請檢查硬體或供電！");
  }
}

void loop() {
  const uint8_t stationCount = WiFi.softAPgetStationNum();
  if (stationCount != previousStationCount) {
    previousStationCount = stationCount;
    Serial.printf("目前已連線裝置：%u 台\n", stationCount);
  }
  delay(200);
}
