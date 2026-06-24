#pragma once
#include <Arduino.h>
#include "../assets/cat_frames.h"

enum CatAnimId {
    ANIM_IDLE_FRONT,
    ANIM_IDLE_BLINK,
    ANIM_TEST_LOOP
};

struct CatFrameRef {
    const CatFrame* frame;
    uint16_t        durationMs;
};

struct CatAnimDef {
    CatAnimId          id;
    const CatFrameRef* frames;
    uint8_t            frameCount;
    bool               loop;
};

class CatAnimationPlayer {
public:
    void begin();
    void play(CatAnimId id, bool restart = true);
    void update(uint32_t nowMs);

    const CatFrame* currentFrame() const;
    uint8_t         currentFrameIndex() const { return _frameIdx; }
    bool            frameChanged() const;
    void            clearFrameChanged();

private:
    const CatAnimDef* _anim     = nullptr;
    uint8_t           _frameIdx = 0;
    uint32_t          _lastMs   = 0;
    bool              _changed  = true;
};
