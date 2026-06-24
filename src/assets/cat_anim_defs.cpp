#include "cat_anim_defs.h"
#include "cat_frames.h"

// ── ANIM_IDLE_FRONT ───────────────────────────────────────────
// Ping-pong: 0→1→2→1 so the cat rocks gently without a hard reset jump.
static const CatFrameRef IDLE_FRONT_FRAMES[] = {
    { &CAT_FRAME_TEST_0, 500 },
    { &CAT_FRAME_TEST_1, 500 },
    { &CAT_FRAME_TEST_2, 500 },
    { &CAT_FRAME_TEST_1, 500 },
};
static const CatAnimDef DEF_IDLE_FRONT = {
    ANIM_IDLE_FRONT, IDLE_FRONT_FRAMES, 4, true
};

// ── ANIM_IDLE_BLINK ───────────────────────────────────────────
// Hold open pose, quick blink frame, return.
static const CatFrameRef IDLE_BLINK_FRAMES[] = {
    { &CAT_FRAME_TEST_0, 900 },
    { &CAT_FRAME_TEST_3, 100 },
    { &CAT_FRAME_TEST_0, 400 },
};
static const CatAnimDef DEF_IDLE_BLINK = {
    ANIM_IDLE_BLINK, IDLE_BLINK_FRAMES, 3, true
};

// ── ANIM_TEST_LOOP ────────────────────────────────────────────
// Cycles all 4 test frames at equal speed — useful for verifying frames.
static const CatFrameRef TEST_LOOP_FRAMES[] = {
    { &CAT_FRAME_TEST_0, 300 },
    { &CAT_FRAME_TEST_1, 300 },
    { &CAT_FRAME_TEST_2, 300 },
    { &CAT_FRAME_TEST_3, 300 },
};
static const CatAnimDef DEF_TEST_LOOP = {
    ANIM_TEST_LOOP, TEST_LOOP_FRAMES, 4, true
};

const CatAnimDef* getCatAnimDef(CatAnimId id) {
    switch (id) {
        case ANIM_IDLE_BLINK: return &DEF_IDLE_BLINK;
        case ANIM_TEST_LOOP:  return &DEF_TEST_LOOP;
        default:              return &DEF_IDLE_FRONT;
    }
}
