#include "input/input.h"
#include <Arduino.h>

InputManager inputManager;

void setup() { Serial.begin(9600); }

void debugButtonsState(uint8_t state) {
  if ((state & DIRECTION_UP) != 0) Serial.print("UP ");
  if ((state & DIRECTION_RIGHT) != 0) Serial.print("RIGHT ");
  if ((state & DIRECTION_DOWN) != 0) Serial.print("DOWN ");
  if ((state & DIRECTION_LEFT) != 0) Serial.print("LEFT ");
  if (state != 0) Serial.println();
}

void loop() {
  inputManager.update();
  uint8_t latestPressed = inputManager.getLatestPressedButton();
  if (latestPressed != 0) {
    Serial.print("Latest pressed: ");
    debugButtonsState(latestPressed);
  };
  // uint8_t justPressed = inputManager.getJustPressedButton();
  // uint8_t justReleased = inputManager.getJustReleasedButton();
  // if (justPressed != 0) {
  //   Serial.print("Just pressed: ");
  //   debugButtonsState(justPressed);
  // }
  // if (justReleased != 0) {
  //   Serial.print("Just released: ");
  //   debugButtonsState(justReleased);
  // }
}