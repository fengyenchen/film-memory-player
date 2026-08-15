#pragma once

#include <Arduino.h>

void setBacklight(bool on);
void drawCentered(const String &text, int16_t y, uint8_t size, uint16_t color);
void showMessage(const String &title, const String &detail = "");
