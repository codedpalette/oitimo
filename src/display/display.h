#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Waveshare_ILI9486.h>

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define BLACK 0x0000

class DisplayManager {
public:
  void setup();
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

private:
  Waveshare_ILI9486 _display;
};

#endif