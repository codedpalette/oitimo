#include <Arduino.h>
#include <DIYables_TFT_SPI.h>

#define TFT_CS_PIN 8
#define TFT_DC_PIN 10
#define TFT_RST_PIN 9

#define WIDTH 320
#define HEIGHT 480

#define BLACK DIYables_TFT_SPI::colorRGB(0, 0, 0)
#define BLUE DIYables_TFT_SPI::colorRGB(0, 0, 255)
#define RED DIYables_TFT_SPI::colorRGB(255, 0, 0)
#define GREEN DIYables_TFT_SPI::colorRGB(0, 255, 0)
#define ORANGE DIYables_TFT_SPI::colorRGB(255, 165, 0)
#define PINK DIYables_TFT_SPI::colorRGB(255, 192, 203)
#define VIOLET DIYables_TFT_SPI::colorRGB(148, 0, 211)
#define TURQUOISE DIYables_TFT_SPI::colorRGB(64, 224, 208)
#define WHITE DIYables_TFT_SPI::colorRGB(255, 255, 255)

DIYables_ILI9488_SPI TFT_display(WIDTH, HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, Arduino!");

  TFT_display.begin();
  TFT_display.setRotation(2); // Portrait flipped (screen pins on top)
  TFT_display.fillScreen(WHITE);
}

void loop() {
  TFT_display.fillCircle(100, 100, 30, RED);
  delay(1000);
}