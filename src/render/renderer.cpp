#include "renderer.h"
#include "../board/board_display.h"
#include "../board/board_config.h"
#include "../assets/cat_frames.h"
#include "../assets/grass_bg.h"
#include <esp_heap_caps.h>

static const uint16_t W = CAT_FRAME_W * CAT_SCALE;  // 192
static const uint16_t H = CAT_FRAME_H * CAT_SCALE;  // 192

// Limit real display refresh rate. CatPet::update() can still run every loop,
// but the screen will be touched at most once every 100 ms.
static const uint32_t RENDER_INTERVAL_MS = 100;

static int16_t   _lx = -1;      // last rendered x
static int16_t   _ly = -1;      // last rendered y
static uint32_t  _lastRenderMs = 0;

// Reusable compositing buffer. It stores the dirty rectangle, not only W×H.
// Usually the dirty rect is about 192×192 to 220×220, but it can grow if needed.
static uint16_t* _buf = nullptr;
static size_t    _bufPixels = 0;

// ── Small helpers ────────────────────────────────────────────────────────────

static inline int16_t i16min(int16_t a, int16_t b) { return a < b ? a : b; }
static inline int16_t i16max(int16_t a, int16_t b) { return a > b ? a : b; }

static bool ensureBuffer(size_t pixels) {
    if (_buf && _bufPixels >= pixels) return true;

    if (_buf) {
        heap_caps_free(_buf);
        _buf = nullptr;
        _bufPixels = 0;
    }

    size_t bytes = pixels * sizeof(uint16_t);

    // Prefer PSRAM. This keeps the renderer from consuming scarce internal RAM.
    _buf = (uint16_t*)heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    // Fallback: try normal 8-bit capable heap. This may fail for big dirty rects,
    // but it gives smaller rects a chance when PSRAM is not configured correctly.
    if (!_buf) {
        _buf = (uint16_t*)heap_caps_malloc(bytes, MALLOC_CAP_8BIT);
    }

    if (!_buf) {
        Serial.printf("renderer: buffer alloc failed, pixels=%u bytes=%u\n",
                      (unsigned)pixels, (unsigned)bytes);
        return false;
    }

    _bufPixels = pixels;
    return true;
}

// Copy a grass/background rectangle into _buf.
// _buf row stride is rw, because _buf represents only this dirty rect.
static void loadGrassRect(int16_t rx, int16_t ry, uint16_t rw, uint16_t rh) {
    for (uint16_t y = 0; y < rh; y++) {
        const uint16_t* src = GRASS_BG + (uint32_t)(ry + y) * SCREEN_W + rx;
        uint16_t*       dst = _buf     + (uint32_t)y        * rw;
        for (uint16_t x = 0; x < rw; x++) {
            dst[x] = pgm_read_word(src + x);
        }
    }
}

// Overlay the scaled sprite into _buf. Transparent mask pixels are skipped.
static void overlaySpriteInRect(const CatFrame* f,
                                int16_t sx, int16_t sy,
                                int16_t rx, int16_t ry,
                                uint16_t rw, uint16_t rh) {
    if (!f) return;

    for (uint16_t py = 0; py < f->height; py++) {
        for (uint16_t px = 0; px < f->width; px++) {
            uint16_t idx = py * f->width + px;
            if (!pgm_read_byte(&f->mask[idx])) continue;

            uint16_t color = pgm_read_word(&f->pixels[idx]);
            int16_t x0 = sx + (int16_t)px * CAT_SCALE;
            int16_t y0 = sy + (int16_t)py * CAT_SCALE;

            for (uint8_t dy = 0; dy < CAT_SCALE; dy++) {
                int16_t yy = y0 + dy;
                if (yy < ry || yy >= ry + (int16_t)rh) continue;

                uint16_t* row = _buf + (uint32_t)(yy - ry) * rw;
                for (uint8_t dx = 0; dx < CAT_SCALE; dx++) {
                    int16_t xx = x0 + dx;
                    if (xx < rx || xx >= rx + (int16_t)rw) continue;
                    row[xx - rx] = color;
                }
            }
        }
    }
}

// No-buffer fallback: restore grass line by line, then draw only opaque pixels.
// It is slower than the buffered path, but it does not paint transparent pixels black.
static void drawFallbackNoBlack(Arduino_GFX* gfx,
                                int16_t rx, int16_t ry,
                                uint16_t rw, uint16_t rh,
                                const CatFrame* f,
                                int16_t sx, int16_t sy) {
    uint16_t rowBuf[SCREEN_W];

    for (uint16_t y = 0; y < rh; y++) {
        const uint16_t* src = GRASS_BG + (uint32_t)(ry + y) * SCREEN_W + rx;
        for (uint16_t x = 0; x < rw; x++) {
            rowBuf[x] = pgm_read_word(src + x);
        }
        gfx->draw16bitRGBBitmap(rx, ry + y, rowBuf, rw, 1);
    }

    if (!f) return;

    for (uint16_t py = 0; py < f->height; py++) {
        for (uint16_t px = 0; px < f->width; px++) {
            uint16_t idx = py * f->width + px;
            if (!pgm_read_byte(&f->mask[idx])) continue;

            gfx->fillRect(sx + px * CAT_SCALE,
                          sy + py * CAT_SCALE,
                          CAT_SCALE,
                          CAT_SCALE,
                          pgm_read_word(&f->pixels[idx]));
        }
    }
}

// ── Public API ───────────────────────────────────────────────────────────────

void rendererBegin() {
    _lx = -1;
    _ly = -1;
    _lastRenderMs = 0;

    if (_buf) {
        heap_caps_free(_buf);
        _buf = nullptr;
        _bufPixels = 0;
    }

    // Pre-allocate the common case: one cat-sized dirty rectangle.
    // Larger dirty rectangles will be allocated on demand.
    ensureBuffer((size_t)W * H);
}

void rendererDrawStaticUI() {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) return;

    // Blit full grass background row by row.
    uint16_t rowBuf[SCREEN_W];
    for (uint16_t y = 0; y < SCREEN_H; y++) {
        const uint16_t* src = GRASS_BG + (uint32_t)y * SCREEN_W;
        for (uint16_t x = 0; x < SCREEN_W; x++) {
            rowBuf[x] = pgm_read_word(src + x);
        }
        gfx->draw16bitRGBBitmap(0, y, rowBuf, SCREEN_W, 1);
    }

    _lx = -1;
    _ly = -1;
    _lastRenderMs = 0;
}

void rendererDrawCatPet(CatPet& pet) {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) {
        pet.clearDirty();
        return;
    }

    int16_t nx = pet.x();
    int16_t ny = pet.y();
    bool firstDraw = (_lx < 0 || _ly < 0);
    bool posChanged = (!firstDraw && (nx != _lx || ny != _ly));

    if (!pet.needsRedraw() && !posChanged && !firstDraw) return;

    uint32_t now = millis();

    // Throttle display refresh only. Do not clear dirty here; the next allowed
    // refresh will draw the latest cat frame and latest cat position.
    if (!firstDraw && (uint32_t)(now - _lastRenderMs) < RENDER_INTERVAL_MS) {
        return;
    }

    // Dirty rectangle = union of old rendered cat rect and new cat rect.
    // This avoids two visible operations: "erase old cat" then "draw new cat".
    int16_t rx1, ry1, rx2, ry2;
    if (firstDraw) {
        rx1 = nx;
        ry1 = ny;
        rx2 = nx + W;
        ry2 = ny + H;
    } else {
        rx1 = i16min(_lx, nx);
        ry1 = i16min(_ly, ny);
        rx2 = i16max((int16_t)(_lx + W), (int16_t)(nx + W));
        ry2 = i16max((int16_t)(_ly + H), (int16_t)(ny + H));
    }

    // Clip defensively to screen bounds.
    if (rx1 < 0) rx1 = 0;
    if (ry1 < 0) ry1 = 0;
    if (rx2 > SCREEN_W) rx2 = SCREEN_W;
    if (ry2 > SCREEN_H) ry2 = SCREEN_H;

    uint16_t rw = (uint16_t)(rx2 - rx1);
    uint16_t rh = (uint16_t)(ry2 - ry1);
    if (rw == 0 || rh == 0) return;

    const CatFrame* f = pet.currentFrame();
    size_t needPixels = (size_t)rw * rh;

    if (ensureBuffer(needPixels)) {
        // Compose background + cat fully off-screen, then do one display blit.
        // This is the important anti-flicker change.
        loadGrassRect(rx1, ry1, rw, rh);
        overlaySpriteInRect(f, nx, ny, rx1, ry1, rw, rh);
        gfx->draw16bitRGBBitmap(rx1, ry1, _buf, rw, rh);
    } else {
        drawFallbackNoBlack(gfx, rx1, ry1, rw, rh, f, nx, ny);
    }

    _lx = nx;
    _ly = ny;
    _lastRenderMs = now;
    pet.clearDirty();
}
