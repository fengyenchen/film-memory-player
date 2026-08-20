#include "BatteryMonitor.h"

#include "Config.h"

namespace {
struct BatteryPoint {
  uint16_t millivolts;
  uint8_t percent;
};

constexpr BatteryPoint BATTERY_CURVE[] = {
    {3300, 0},  {3500, 5},  {3600, 10}, {3700, 25}, {3750, 40},
    {3800, 55}, {3900, 70}, {4000, 80}, {4100, 90}, {4200, 100},
};

uint8_t voltageToPercent(uint16_t millivolts) {
  if (millivolts <= BATTERY_CURVE[0].millivolts) return 0;
  const size_t last = sizeof(BATTERY_CURVE) / sizeof(BATTERY_CURVE[0]) - 1;
  if (millivolts >= BATTERY_CURVE[last].millivolts) return 100;

  for (size_t index = 1; index <= last; ++index) {
    const BatteryPoint &high = BATTERY_CURVE[index];
    if (millivolts > high.millivolts) continue;
    const BatteryPoint &low = BATTERY_CURVE[index - 1];
    return low.percent +
           uint32_t(millivolts - low.millivolts) * (high.percent - low.percent) /
               (high.millivolts - low.millivolts);
  }
  return 0;
}
} // namespace

void initBatteryMonitor() {
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
  pinMode(PIN_BATTERY_ADC, INPUT);
  analogSetPinAttenuation(PIN_BATTERY_ADC, ADC_11db);
#endif
}

bool readBatteryStatus(float &voltage, uint8_t &percent) {
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
  uint32_t measuredMillivolts = 0;
  for (uint8_t sample = 0; sample < BATTERY_SAMPLE_COUNT; ++sample) {
    measuredMillivolts += analogReadMilliVolts(PIN_BATTERY_ADC);
    delayMicroseconds(200);
  }

  const float batteryMillivolts =
      float(measuredMillivolts) * BATTERY_DIVIDER_RATIO / BATTERY_SAMPLE_COUNT;
  voltage = batteryMillivolts / 1000.0f;
  if (voltage < 2.5f || voltage > 4.6f) {
    percent = 0;
    return false;
  }
  percent = voltageToPercent(uint16_t(batteryMillivolts + 0.5f));
  return true;
#else
  voltage = 0.0f;
  percent = 0;
  return false;
#endif
}
