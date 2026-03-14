#include "display/display.h"
#include "input/input.h"
#include <Arduino.h>

#define CELL_SIZE 20
#define WORLD_WIDTH (DISPLAY_WIDTH / CELL_SIZE)
#define WORLD_HEIGHT (DISPLAY_HEIGHT / CELL_SIZE)

InputManager inputManager;
DisplayManager displayManager;

uint8_t currentX = WORLD_WIDTH / 2;
uint8_t currentY = WORLD_HEIGHT / 2;

void setup() {
  Serial.begin(9600);
  displayManager.setup();
  displayManager.fillRect(currentX * CELL_SIZE, currentY * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
}

void loop() {
  inputManager.update();
  uint8_t latestPressed = inputManager.getLatestPressedButton();
  if (latestPressed != 0) {
    Serial.print("Latest pressed: ");
    inputManager.debugPrintButtons();

    displayManager.fillRect(currentX * CELL_SIZE, currentY * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
    if (latestPressed == DIRECTION_UP) currentY = (currentY - 1 + WORLD_HEIGHT) % WORLD_HEIGHT;
    if (latestPressed == DIRECTION_LEFT) currentX = (currentX - 1 + WORLD_WIDTH) % WORLD_WIDTH;
    if (latestPressed == DIRECTION_DOWN) currentY = (currentY + 1) % WORLD_HEIGHT;
    if (latestPressed == DIRECTION_RIGHT) currentX = (currentX + 1) % WORLD_WIDTH;
    displayManager.fillRect(currentX * CELL_SIZE, currentY * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
  }
}