#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#define DIRECTION_UP 0b0001
#define DIRECTION_RIGHT 0b0010
#define DIRECTION_DOWN 0b0100
#define DIRECTION_LEFT 0b1000

class InputManager {
public:
  void update();
  uint8_t getLatestPressedButton();
  uint8_t getJustPressedButton();
  uint8_t getJustReleasedButton();

private:
  uint8_t _pressedState = 0; // highest 4 bits: previous state, lowest 4 bits: current state
  uint8_t _pressStack = 0;   // 4x 2-bit slots
  uint8_t _getRawButtonsState();
  uint8_t _slotGet(uint8_t i);
  void _stackPush(uint8_t index);
  void _stackRemove(uint8_t i);
};

#endif