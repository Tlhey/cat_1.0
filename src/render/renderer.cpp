#include "renderer.h"
#include "sprite_renderer.h"
#include "../board/board_display.h"
#include "../board/board_config.h"

static const uint16_t W = CAT_FRAME_W * CAT_SCALE;  // 192
static const uint16_t H = CAT_FRAME_H * CAT_SCALE;  // 192

// -1 means "no previous cat drawn yet"
static int16_t _lx = -1;
static int16_t _ly = -1;

void rendererBegin() { _lx = -1; _ly = -1; }

void rendererDrawStaticUI() {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) return;
    gfx->fillScreen(0x0000);   // entire screen black, once
    _lx = -1; _ly = -1;       // no previous cat
}

void rendererDrawCatPet(CatPet& pet) {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) { pet.clearDirty(); return; }

    int16_t nx = pet.x();
    int16_t ny = pet.y();
    bool posChanged = (nx != _lx || ny != _ly);

    if (!pet.needsRedraw() && !posChanged) return;

    // Clear old cat position
    if (_lx >= 0 && posChanged) {
        gfx->fillRect(_lx, _ly, W, H, 0x0000);
    }

    // Clear current position, then draw sprite
    gfx->fillRect(nx, ny, W, H, 0x0000);
    const CatFrame* f = pet.currentFrame();
    if (f) drawSpriteNearest(gfx, *f, nx, ny, CAT_SCALE);

    _lx = nx;
    _ly = ny;
    pet.clearDirty();
}
