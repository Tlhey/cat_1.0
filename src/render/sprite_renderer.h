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

// Buffered sprite draw: fills a scale*32 × scale*32 buffer with bg, overlays
// sprite pixels, then sends the whole buffer in one draw16bitRGBBitmap call.
// Uses a static PSRAM buffer allocated on first call.
void drawSpriteBuffered(
    Arduino_GFX*     gfx,
    const CatFrame&  frame,
    int16_t          x,
    int16_t          y,
    uint8_t          scale,
    uint16_t         bg
);
