#pragma once

#include <Arduino.h>

void initBatteryMonitor();
bool readBatteryStatus(float &voltage, uint8_t &percent);
