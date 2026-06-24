#include "board_touch.h"
#include <Wire.h>

#define CST816_ADDR  0x15

static bool _prevTouching = false;

void touchBegin() {
    pinMode(TP_INT, INPUT);  // board has external pull-up
    _prevTouching = false;
}

// Returns true exactly once per touch-down (not while held).
bool touchPoll(int16_t& x, int16_t& y) {
    bool touching = (digitalRead(TP_INT) == LOW);

    if (!touching) {
        _prevTouching = false;
        return false;
    }
    if (_prevTouching) return false; // already reported this touch

    _prevTouching = true;

    Wire.beginTransmission(CST816_ADDR);
    Wire.write(0x01);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom((uint8_t)CST816_ADDR, (uint8_t)6) != 6) return false;

    Wire.read();                     // gesture
    uint8_t fingers = Wire.read();   // finger count
    uint8_t xh      = Wire.read();   // X[11:8] in bits 3:0
    uint8_t xl      = Wire.read();   // X[7:0]
    uint8_t yh      = Wire.read();   // Y[11:8] in bits 3:0
    uint8_t yl      = Wire.read();   // Y[7:0]

    if (fingers == 0) return false;
    x = (int16_t)(((uint16_t)(xh & 0x0F) << 8) | xl);
    y = (int16_t)(((uint16_t)(yh & 0x0F) << 8) | yl);
    return true;
}
