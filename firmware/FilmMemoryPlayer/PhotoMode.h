#pragma once

#include <Arduino.h>

bool tftJpegOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
void drawPhoto();
void scanPhotos();
void changePhoto(int direction);
