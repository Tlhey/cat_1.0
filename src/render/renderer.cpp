#include "renderer.h"
#include "../board/board_display.h"
#include "../board/board_config.h"
#include "../assets/cat_frames.h"
#include "../assets/grass_bg.h"

static const uint16_t W = CAT_FRAME_W * CAT_SCALE;  // 192
static const uint16_t H = CAT_FRAME_H * CAT_SCALE;  // 192

static int16_t   _lx  = -1;
static int16_t   _ly  = -1;
static uint16_t* _buf = nullptr;  // PSRAM composite buffer (W×H pixels)

// ── Compositing helpers ───────────────────────────────────────────────────────

// Copy the W×H grass region at screen position (sx, sy) into _buf.
static void loadGrass(int16_t sx, int16_t sy) {
    for (uint16_t r = 0; r < H; r++) {
        const uint16_t* src = GRASS_BG + (uint32_t)(sy + r) * SCREEN_W + sx;
        uint16_t*       dst = _buf     + (uint32_t)r         * W;
        for (uint16_t c = 0; c < W; c++) dst[c] = pgm_read_word(src + c);
    }
}

// Overlay sprite pixels (nearest-neighbour ×CAT_SCALE) onto _buf.
// Transparent mask pixels are skipped — grass already in buffer shows through.
static void overlaySprite(const CatFrame* f) {
    for (uint16_t py = 0; py < f->height; py++) {
        for (uint16_t px = 0; px < f->width; px++) {
            uint16_t idx = py * f->width + px;
            if (!pgm_read_byte(&f->mask[idx])) continue;
            uint16_t color = pgm_read_word(&f->pixels[idx]);
            uint16_t bx0 = px * CAT_SCALE;
            uint16_t by0 = py * CAT_SCALE;
            for (uint8_t dy = 0; dy < CAT_SCALE; dy++) {
                uint16_t* row = _buf + (by0 + dy) * W + bx0;
                for (uint8_t dx = 0; dx < CAT_SCALE; dx++) row[dx] = color;
            }
        }
    }
}

// ── Public API ────────────────────────────────────────────────────────────────

void rendererBegin() {
    _lx = _ly = -1;
    // W×H×2 bytes in PSRAM: 192×192×2 = 73,728 bytes
    _buf = (uint16_t*)heap_caps_malloc((size_t)W * H * 2, MALLOC_CAP_SPIRAM);
    if (!_buf) Serial.println("rendererBegin: PSRAM alloc failed");
}

void rendererDrawStaticUI() {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) return;

    // Blit full grass background row by row.
    // Row buffer: SCREEN_W × 2 = 736 bytes — safe on stack.
    uint16_t rowBuf[SCREEN_W];
    for (uint16_t y = 0; y < SCREEN_H; y++) {
        const uint16_t* src = GRASS_BG + (uint32_t)y * SCREEN_W;
        for (uint16_t x = 0; x < SCREEN_W; x++) rowBuf[x] = pgm_read_word(src + x);
        gfx->draw16bitRGBBitmap(0, y, rowBuf, SCREEN_W, 1);
    }
    _lx = _ly = -1;
}

void rendererDrawCatPet(CatPet& pet) {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) { pet.clearDirty(); return; }

    int16_t nx = pet.x();
    int16_t ny = pet.y();
    bool posChanged = (nx != _lx || ny != _ly);

    if (!pet.needsRedraw() && !posChanged) return;

    if (!_buf) {
        // No PSRAM: fallback to solid-colour erase + per-pixel sprite
        if (_lx >= 0 && posChanged) gfx->fillRect(_lx, _ly, W, H, COLOR_BG);
        gfx->fillRect(nx, ny, W, H, COLOR_BG);
        const CatFrame* f = pet.currentFrame();
        if (f) {
            for (uint16_t py = 0; py < f->height; py++)
                for (uint16_t px = 0; px < f->width; px++) {
                    uint16_t i = py * f->width + px;
                    if (!pgm_read_byte(&f->mask[i])) continue;
                    gfx->fillRect(nx + px*CAT_SCALE, ny + py*CAT_SCALE,
                                  CAT_SCALE, CAT_SCALE,
                                  pgm_read_word(&f->pixels[i]));
                }
        }
        _lx = nx; _ly = ny;
        pet.clearDirty();
        return;
    }

    // ── PSRAM compositing path (no fillRect, no black artifacts) ─────────────

    // Restore grass at old position (overwrites the sprite that was there)
    if (_lx >= 0 && posChanged) {
        loadGrass(_lx, _ly);
        gfx->draw16bitRGBBitmap(_lx, _ly, _buf, W, H);
    }

    // Composite grass + current sprite frame at new position
    loadGrass(nx, ny);
    const CatFrame* f = pet.currentFrame();
    if (f) overlaySprite(f);
    gfx->draw16bitRGBBitmap(nx, ny, _buf, W, H);

    _lx = nx;
    _ly = ny;
    pet.clearDirty();
}
