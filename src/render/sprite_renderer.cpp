#include "sprite_renderer.h"
#include "../board/board_config.h"

void drawSpriteNearest(
    Arduino_GFX*    gfx,
    const CatFrame& frame,
    int16_t         x,
    int16_t         y,
    uint8_t         scale
) {
    if (!gfx || scale == 0) return;

    for (uint16_t py = 0; py < frame.height; py++) {
        for (uint16_t px = 0; px < frame.width; px++) {
            uint16_t idx = py * frame.width + px;
            if (!pgm_read_byte(&frame.mask[idx])) continue;
            uint16_t color = pgm_read_word(&frame.pixels[idx]);
            gfx->fillRect(
                x + (int16_t)(px * scale),
                y + (int16_t)(py * scale),
                scale, scale,
                color
            );
        }
    }
}
