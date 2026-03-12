#include <Arduino.h>

#define PIN_UP A5
#define PIN_RIGHT A4
#define PIN_LEFT A3
#define PIN_DOWN A2

#define DIRECTION_UP 0b0001
#define DIRECTION_RIGHT 0b0010
#define DIRECTION_DOWN 0b0100
#define DIRECTION_LEFT 0b1000

uint8_t previousButtonState = 0;
uint8_t getRawButtonsState();
uint8_t latestButton(uint8_t state);

void setup() { Serial.begin(9600); }

void loop() {
  uint8_t rawButtonsState = getRawButtonsState();
  uint8_t latest = latestButton(rawButtonsState);
  if ((latest & DIRECTION_UP) != 0) Serial.print("UP ");
  if ((latest & DIRECTION_RIGHT) != 0) Serial.print("RIGHT ");
  if ((latest & DIRECTION_DOWN) != 0) Serial.print("DOWN ");
  if ((latest & DIRECTION_LEFT) != 0) Serial.print("LEFT ");
  if (latest != 0) Serial.println();
}

uint8_t _stack = 0; // 4x 2-bit slots
uint8_t _stackLen = 0;

// Read index from slot j
static inline uint8_t _slotGet(uint8_t j) { return (_stack >> (j * 2)) & 0x03; }

// Push index onto top of stack
static inline void _stackPush(uint8_t index) {
  _stack |= (index << (_stackLen * 2));
  _stackLen++;
}

// Remove slot j, shift everything above it down
static void _stackRemove(uint8_t j) {
  uint8_t lower = _stack & ((1 << (j * 2)) - 1); // bits below j, unchanged
  uint8_t upper = (_stack >> ((j + 1) * 2))
                  << (j * 2); // bits above j, shifted down
  _stack = lower | upper;
  _stackLen--;
}

uint8_t latestButton(uint8_t state) {
  uint8_t pressed = state & ~previousButtonState;
  uint8_t released = ~state & previousButtonState;

  for (uint8_t i = 0; i < 4; i++) {
    if (pressed & (1 << i)) _stackPush(i);
  }

  for (uint8_t i = 0; i < 4; i++) {
    if (released & (1 << i)) {
      for (uint8_t j = 0; j < _stackLen; j++) {
        if (_slotGet(j) == i) {
          _stackRemove(j);
          break;
        }
      }
    }
  }

  previousButtonState = state;
  return _stackLen ? (1 << _slotGet(_stackLen - 1)) : 0;
}

uint8_t getRawButtonsState() {
  const int valueUp = analogRead(PIN_UP);
  const int valueRight = analogRead(PIN_RIGHT);
  const int valueLeft = analogRead(PIN_LEFT);
  const int valueDown = analogRead(PIN_DOWN);

  uint8_t direction = 0b0000;
  if (valueUp > 512) direction |= DIRECTION_UP;
  if (valueRight > 512) direction |= DIRECTION_RIGHT;
  if (valueDown > 512) direction |= DIRECTION_DOWN;
  if (valueLeft > 512) direction |= DIRECTION_LEFT;
  return direction;
}