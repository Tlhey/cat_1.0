#include "renderer.h"
#include "sprite_renderer.h"
#include "../board/board_display.h"
#include "../board/board_config.h"

static const uint16_t CAT_RENDER_W = CAT_FRAME_W * CAT_SCALE;   // 192
static const uint16_t CAT_RENDER_H = CAT_FRAME_H * CAT_SCALE;   // 192
static const int16_t  CAT_X        = (SCREEN_W - (int16_t)CAT_RENDER_W) / 2;  // 88
static const int16_t  CAT_Y        = 120;

// Last drawn cat position for dirty-rect clearing in Phase 2.
static int16_t _lastCatX = CAT_X;
static int16_t _lastCatY = CAT_Y;

void rendererBegin() {
    _lastCatX = CAT_X;
    _lastCatY = CAT_Y;
}

void rendererDrawStaticUI() {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx) return;

    gfx->fillScreen(COLOR_BG);

    gfx->setTextColor(COLOR_TITLE);
    gfx->setTextSize(2);
    gfx->setCursor(100, 40);
    gfx->print("CatPet Phase 2");

    gfx->setTextColor(COLOR_INFO);
    gfx->setTextSize(2);
    gfx->setCursor(64, 400);
    gfx->print("Random idle active");
}

void rendererDrawCat(const CatFrame* frame) {
    Arduino_GFX* gfx = getDisplay();
    if (!gfx || !frame) return;

    gfx->fillRect(CAT_X, CAT_Y, CAT_RENDER_W, CAT_RENDER_H, COLOR_BG);
    drawSpriteNearest(gfx, *frame, CAT_X, CAT_Y, CAT_SCALE);
}

void rendererUpdate(CatAnimationPlayer& anim) {
    if (anim.frameChanged()) {
        rendererDrawCat(anim.currentFrame());
        anim.clearFrameChanged();
    }
}

// ── Phase 2 ───────────────────────────────────────────────────────────────────
void rendererDrawCatActor(CatActor& actor) {
    if (!actor.needsRedraw()) return;

    Arduino_GFX* gfx = getDisplay();
    if (!gfx) { actor.clearDirty(); return; }

    const CatFrame* frame = actor.currentFrame();
    if (!frame) { actor.clearDirty(); return; }

    int16_t newX = actor.x();
    int16_t newY = actor.y();

    // Clear the old position if the cat moved.
    if (newX != _lastCatX || newY != _lastCatY) {
        gfx->fillRect(_lastCatX, _lastCatY, CAT_RENDER_W, CAT_RENDER_H, COLOR_BG);
    }

    // Clear the current position (handles frame-only changes on same spot too).
    gfx->fillRect(newX, newY, CAT_RENDER_W, CAT_RENDER_H, COLOR_BG);

    drawSpriteNearest(gfx, *frame, newX, newY, CAT_SCALE);

    _lastCatX = newX;
    _lastCatY = newY;

    actor.clearDirty();
}
