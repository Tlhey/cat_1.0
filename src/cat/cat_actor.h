#pragma once
#include <Arduino.h>
#include "cat_action.h"
#include "../board/board_config.h"
#include "../assets/cat_frames.h"

// ── Cat movement bounds ───────────────────────────────────────
// CAT_FRAME_W/H = 32 (from cat_frames.h), CAT_SCALE = 6 (from board_config.h)
#define CAT_DRAW_W  (CAT_FRAME_W * CAT_SCALE)          // 192
#define CAT_DRAW_H  (CAT_FRAME_H * CAT_SCALE)          // 192
#define CAT_INIT_X  ((SCREEN_W - CAT_DRAW_W) / 2)      // 88
#define CAT_INIT_Y  120
#define CAT_MIN_X   0
#define CAT_MAX_X   (SCREEN_W - CAT_DRAW_W)            // 176
#define CAT_MIN_Y   90                                  // keep clear of title text (y=40..56)
#define CAT_MAX_Y   (SCREEN_H - 70 - CAT_DRAW_H)       // 186

class CatActor {
public:
    void begin();

    void playAction(CatActionId id, bool restart = true);
    void update(uint32_t nowMs);

    const CatFrame*   currentFrame()    const;
    int16_t           x()               const { return _posX; }
    int16_t           y()               const { return _posY; }

    bool              frameChanged()    const { return _dirtyFrame; }
    bool              positionChanged() const { return _dirtyPos; }
    bool              needsRedraw()     const { return _dirtyFrame || _dirtyPos; }
    void              clearDirty();

    bool              actionFinished()  const;
    CatActionId       currentActionId() const;
    CatActionCategory currentCategory() const;
    CatEmotion        currentEmotion()  const;

private:
    int16_t             _posX = CAT_INIT_X;
    int16_t             _posY = CAT_INIT_Y;

    const CatActionDef* _action       = nullptr;
    uint8_t             _frameIdx     = 0;
    uint8_t             _cyclesPlayed = 0;
    uint32_t            _lastFrameMs  = 0;

    bool                _dirtyFrame   = true;
    bool                _dirtyPos     = false;
};
