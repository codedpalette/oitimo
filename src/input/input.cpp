#include "input.h"
#include <Arduino.h>

#define PIN_UP A5
#define PIN_RIGHT A4
#define PIN_LEFT A3
#define PIN_DOWN A2

uint8_t _countBits(uint8_t x) {
  // Step 1: count bits within each 2-bit pair, simultaneously
  // 0x05 = 0b0101 — masks the low bit of each pair after shifting
  // e.g. for 0b1111 → 0b1111 - (0b0111 & 0b0101) = 0b1111 - 0b0101 = 0b1010
  // meaning 2 (0b10) bits are set in each pair
  x -= (x >> 1) & 0x05;

  // Step 2: add the two 2-bit results together
  x = (x & 0x03) + ((x >> 2) & 0x03);
  return x;
}

// Runs every loop
void InputManager::update() {
  // Stack length is the number of active buttons before this update
  // Which is the number of active bits in lower nibble of state
  uint8_t stackLen = _countBits(_pressedState & 0x0F);
  _pressedState = (_pressedState << 4) | _getRawButtonsState();

  uint8_t justPressed = getJustPressedButton();
  uint8_t justReleased = getJustReleasedButton();

  // Remove released buttons from stack
  for (uint8_t i = 0; i < 4; i++) {
    if (justReleased & (1 << i)) {
      for (uint8_t j = 0; j < stackLen; j++) {
        if (_slotGet(j) == i) {
          _stackRemove(j);
          break;
        }
      }
    }
  }

  // Push newly pressed buttons onto top of stack
  for (uint8_t i = 0; i < 4; i++) {
    if (justPressed & (1 << i)) {
      _stackPush(i);
    }
  }
}

uint8_t InputManager::getLatestPressedButton() {
  uint8_t stackLen = _countBits(_pressedState & 0x0F);
  return stackLen ? (1 <<                  // Shift 1 left by index of a latest pressed button to get it's mask
                     (_pressStack & 0x03)) // Lowest 2 bits of stack is index of latest pressed button
                  : 0;
}

uint8_t InputManager::getJustPressedButton() {
  return _pressedState           // 4 highest bits: previous state, 4 lowest bits: current state
         & ~(_pressedState >> 4) // 4 lowest bits set to 1 if currently pressed but not previously
         & 0x0F;                 // set 4 highest bits to 0;
}

uint8_t InputManager::getJustReleasedButton() {
  return ~_pressedState         // 4 highest bits: previous state, 4 lowest bits: current state
         & (_pressedState >> 4) // 4 lowest bits set to 1 if currently released but not previously
         & 0x0F;                // set 4 highest bits to 0;
}

uint8_t InputManager::_getRawButtonsState() {
  uint8_t direction = 0b0000;
  if (analogRead(PIN_UP) > 512) direction |= DIRECTION_UP;
  if (analogRead(PIN_RIGHT) > 512) direction |= DIRECTION_RIGHT;
  if (analogRead(PIN_DOWN) > 512) direction |= DIRECTION_DOWN;
  if (analogRead(PIN_LEFT) > 512) direction |= DIRECTION_LEFT;
  return direction;
}

// Read button index from 2-bit slot i
uint8_t InputManager::_slotGet(uint8_t i) {
  return (_pressStack >> (i * 2)) // Shift slot i to the lowest 2 bits
         & 0x03;                  // Get only the lowest 2 bits
}

// Push button index onto top of stack
void InputManager::_stackPush(uint8_t index) {
  _pressStack = (_pressStack << 2) // Move everything up 1 slot (=2 bits)
                | (index & 0x03);  // Set lowest slot to index encoded in 2 bits
}

// Remove slot i, shift everything above it down
void InputManager::_stackRemove(uint8_t i) {
  uint8_t lower = _pressStack & ((1 << (i * 2)) - 1);        // bits below i, unchanged
  uint8_t upper = (_pressStack >> ((i + 1) * 2)) << (i * 2); // bits above i, shifted down one slot
  _pressStack = lower | upper;
}

void InputManager::debugPrintState() {
  Serial.print("State: ");
  for (int8_t i = 7; i >= 0; i--) {
    Serial.print((_pressedState >> i) & 1);
  }
  Serial.print(" Stack: ");
  for (int8_t i = 7; i >= 0; i--) {
    Serial.print((_pressStack >> i) & 1);
  }
  Serial.println();
}

void InputManager::debugPrintButtons() {
  uint8_t state = getLatestPressedButton();
  if ((state & DIRECTION_UP) != 0) Serial.print("UP ");
  if ((state & DIRECTION_RIGHT) != 0) Serial.print("RIGHT ");
  if ((state & DIRECTION_DOWN) != 0) Serial.print("DOWN ");
  if ((state & DIRECTION_LEFT) != 0) Serial.print("LEFT ");
  if (state != 0) Serial.println();
}