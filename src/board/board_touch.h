#pragma once
#include <Arduino.h>
#include "board_config.h"

// Initialize CST816 touch controller (call after Wire / displayBegin).
void touchBegin();

// Returns true once per touch-DOWN event; fills (x,y) with screen coords.
// Returns false while finger is held or screen is not touched.
bool touchPoll(int16_t& x, int16_t& y);
