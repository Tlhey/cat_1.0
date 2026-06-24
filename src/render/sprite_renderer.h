#pragma once
#include <Arduino_GFX_Library.h>
#include "../assets/cat_frames.h"

// Nearest-neighbour scaled sprite draw. mask=0 pixels are skipped (transparent).
void drawSpriteNearest(
    Arduino_GFX*     gfx,
    const CatFrame&  frame,
    int16_t          x,
    int16_t          y,
    uint8_t          scale
);
