#include "display.h"
#include <Arduino.h>

void DisplayManager::setup() {
  SPI.begin();
  _display.begin();

  TSConfigData data = {85, 925, 67, 964};
  _display.setTsConfigData(data);
  _display.setRotation(3);

  _display.fillScreen(0x0000);
}

void DisplayManager::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  _display.fillRect(x, y, w, h, color);
}