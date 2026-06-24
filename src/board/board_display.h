#pragma once
#include <Arduino_GFX_Library.h>
#include "board_config.h"

bool          displayBegin();
Arduino_GFX*  getDisplay();
void          displayClear(uint16_t color = COLOR_BG);
