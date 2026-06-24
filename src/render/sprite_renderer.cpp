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

// Static PSRAM buffer — allocated once on first call.
// Max size: 32*scale × 32*scale × 2 bytes.  At scale 4: 128×128×2 = 32 KB.
static uint16_t* _spriteBuf    = nullptr;
static uint16_t  _spriteBufSide = 0;  // side length of current buffer

static uint16_t* getSpriteBuf(uint8_t scale) {
    uint16_t side = (uint16_t)CAT_FRAME_W * scale;
    if (_spriteBuf && _spriteBufSide == side) return _spriteBuf;

    free(_spriteBuf);
    _spriteBuf = (uint16_t*)heap_caps_malloc(
        (size_t)side * side * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    _spriteBufSide = _spriteBuf ? side : 0;
    return _spriteBuf;
}

void drawSpriteBuffered(
    Arduino_GFX*    gfx,
    const CatFrame& frame,
    int16_t         x,
    int16_t         y,
    uint8_t         scale,
    uint16_t        bg
) {
    if (!gfx || scale == 0) return;

    uint16_t side = (uint16_t)frame.width * scale;
    uint16_t* buf = getSpriteBuf(scale);
    if (!buf) {
        drawSpriteNearest(gfx, frame, x, y, scale);  // fallback
        return;
    }

    // 1. Fill buffer with background color
    uint32_t n = (uint32_t)side * side;
    for (uint32_t i = 0; i < n; i++) buf[i] = bg;

    // 2. Overlay sprite pixels (nearest-neighbour)
    for (uint16_t py = 0; py < frame.height; py++) {
        for (uint16_t px = 0; px < frame.width; px++) {
            uint16_t idx = py * frame.width + px;
            if (!pgm_read_byte(&frame.mask[idx])) continue;
            uint16_t color = pgm_read_word(&frame.pixels[idx]);
            uint16_t bx0 = px * scale;
            uint16_t by0 = py * scale;
            for (uint8_t dy = 0; dy < scale; dy++) {
                uint16_t* row = buf + (by0 + dy) * side + bx0;
                for (uint8_t dx = 0; dx < scale; dx++) row[dx] = color;
            }
        }
    }

    // 3. Single bitmap blit
    gfx->draw16bitRGBBitmap(x, y, buf, side, side);
}
