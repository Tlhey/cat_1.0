#include "cat_action_defs.h"
#include "cat_frames.h"

// ── Frame duration constants ──────────────────────────────────────────────────
static constexpr uint16_t WALK_MS  = 150;   // ms per walk frame
static constexpr uint16_t SLEEP_MS = 800;   // ms per sleep frame

// ── WALK_RIGHT ────────────────────────────────────────────────────────────────
static const CatFrameRef WALK_RIGHT_FRAMES[] = {
    { &CAT_FRAME_WALK_RIGHT_0, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_1, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_2, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_3, WALK_MS },
};
static const CatActionDef DEF_WALK_RIGHT = {
    ACTION_WALK_RIGHT, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_RIGHT_FRAMES, 4,
    /*canBeRandomIdle=*/true, /*movesCat=*/true,
    /*dx=*/3, /*dy=*/0,
    WALK_MS, /*maxCycles=*/0
};

// ── WALK_LEFT ─────────────────────────────────────────────────────────────────
static const CatFrameRef WALK_LEFT_FRAMES[] = {
    { &CAT_FRAME_WALK_LEFT_0, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_1, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_2, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_3, WALK_MS },
};
static const CatActionDef DEF_WALK_LEFT = {
    ACTION_WALK_LEFT, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_LEFT_FRAMES, 4,
    true, true,
    /*dx=*/-3, /*dy=*/0,
    WALK_MS, 0
};

// ── WALK_DOWN ─────────────────────────────────────────────────────────────────
static const CatFrameRef WALK_DOWN_FRAMES[] = {
    { &CAT_FRAME_WALK_DOWN_0, WALK_MS },
    { &CAT_FRAME_WALK_DOWN_1, WALK_MS },
    { &CAT_FRAME_WALK_DOWN_2, WALK_MS },
    { &CAT_FRAME_WALK_DOWN_3, WALK_MS },
};
static const CatActionDef DEF_WALK_DOWN = {
    ACTION_WALK_DOWN, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_DOWN_FRAMES, 4,
    true, true,
    /*dx=*/0, /*dy=*/3,
    WALK_MS, 0
};

// ── WALK_UP ───────────────────────────────────────────────────────────────────
static const CatFrameRef WALK_UP_FRAMES[] = {
    { &CAT_FRAME_WALK_UP_0, WALK_MS },
    { &CAT_FRAME_WALK_UP_1, WALK_MS },
    { &CAT_FRAME_WALK_UP_2, WALK_MS },
    { &CAT_FRAME_WALK_UP_3, WALK_MS },
};
static const CatActionDef DEF_WALK_UP = {
    ACTION_WALK_UP, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_UP_FRAMES, 4,
    true, true,
    /*dx=*/0, /*dy=*/-3,
    WALK_MS, 0
};

// ── WALK_RIGHT_DOWN ───────────────────────────────────────────────────────────
static const CatFrameRef WALK_RIGHT_DOWN_FRAMES[] = {
    { &CAT_FRAME_WALK_RIGHT_DOWN_0, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_DOWN_1, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_DOWN_2, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_DOWN_3, WALK_MS },
};
static const CatActionDef DEF_WALK_RIGHT_DOWN = {
    ACTION_WALK_RIGHT_DOWN, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_RIGHT_DOWN_FRAMES, 4,
    true, true,
    /*dx=*/2, /*dy=*/2,
    WALK_MS, 0
};

// ── WALK_LEFT_DOWN ────────────────────────────────────────────────────────────
static const CatFrameRef WALK_LEFT_DOWN_FRAMES[] = {
    { &CAT_FRAME_WALK_LEFT_DOWN_0, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_DOWN_1, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_DOWN_2, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_DOWN_3, WALK_MS },
};
static const CatActionDef DEF_WALK_LEFT_DOWN = {
    ACTION_WALK_LEFT_DOWN, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_LEFT_DOWN_FRAMES, 4,
    true, true,
    /*dx=*/-2, /*dy=*/2,
    WALK_MS, 0
};

// ── WALK_RIGHT_UP ─────────────────────────────────────────────────────────────
static const CatFrameRef WALK_RIGHT_UP_FRAMES[] = {
    { &CAT_FRAME_WALK_RIGHT_UP_0, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_UP_1, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_UP_2, WALK_MS },
    { &CAT_FRAME_WALK_RIGHT_UP_3, WALK_MS },
};
static const CatActionDef DEF_WALK_RIGHT_UP = {
    ACTION_WALK_RIGHT_UP, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_RIGHT_UP_FRAMES, 4,
    true, true,
    /*dx=*/2, /*dy=*/-2,
    WALK_MS, 0
};

// ── WALK_LEFT_UP ──────────────────────────────────────────────────────────────
static const CatFrameRef WALK_LEFT_UP_FRAMES[] = {
    { &CAT_FRAME_WALK_LEFT_UP_0, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_UP_1, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_UP_2, WALK_MS },
    { &CAT_FRAME_WALK_LEFT_UP_3, WALK_MS },
};
static const CatActionDef DEF_WALK_LEFT_UP = {
    ACTION_WALK_LEFT_UP, CAT_ACTION_WALK, EMOTION_CURIOUS,
    WALK_LEFT_UP_FRAMES, 4,
    true, true,
    /*dx=*/-2, /*dy=*/-2,
    WALK_MS, 0
};

// ── SLEEP_1_LEFT ──────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_1_LEFT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_1_LEFT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_1_LEFT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_1_LEFT = {
    ACTION_SLEEP_1_LEFT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_1_LEFT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6   // 6 cycles ≈ ~10 s
};

// ── SLEEP_1_RIGHT ─────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_1_RIGHT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_1_RIGHT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_1_RIGHT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_1_RIGHT = {
    ACTION_SLEEP_1_RIGHT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_1_RIGHT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── SLEEP_2_LEFT ──────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_2_LEFT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_2_LEFT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_2_LEFT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_2_LEFT = {
    ACTION_SLEEP_2_LEFT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_2_LEFT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── SLEEP_2_RIGHT ─────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_2_RIGHT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_2_RIGHT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_2_RIGHT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_2_RIGHT = {
    ACTION_SLEEP_2_RIGHT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_2_RIGHT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── SLEEP_3_LEFT ──────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_3_LEFT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_3_LEFT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_3_LEFT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_3_LEFT = {
    ACTION_SLEEP_3_LEFT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_3_LEFT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── SLEEP_3_RIGHT ─────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_3_RIGHT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_3_RIGHT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_3_RIGHT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_3_RIGHT = {
    ACTION_SLEEP_3_RIGHT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_3_RIGHT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── SLEEP_4_LEFT ──────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_4_LEFT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_4_LEFT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_4_LEFT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_4_LEFT = {
    ACTION_SLEEP_4_LEFT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_4_LEFT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── SLEEP_4_RIGHT ─────────────────────────────────────────────────────────────
static const CatFrameRef SLEEP_4_RIGHT_FRAMES[] = {
    { &CAT_FRAME_SLEEP_4_RIGHT_0, SLEEP_MS },
    { &CAT_FRAME_SLEEP_4_RIGHT_1, SLEEP_MS },
};
static const CatActionDef DEF_SLEEP_4_RIGHT = {
    ACTION_SLEEP_4_RIGHT, CAT_ACTION_SLEEP, EMOTION_SLEEPY,
    SLEEP_4_RIGHT_FRAMES, 2,
    true, false,
    0, 0,
    SLEEP_MS, 6
};

// ── Lookup ────────────────────────────────────────────────────────────────────
const CatActionDef* getCatActionDef(CatActionId id) {
    switch (id) {
        case ACTION_WALK_RIGHT:      return &DEF_WALK_RIGHT;
        case ACTION_WALK_LEFT:       return &DEF_WALK_LEFT;
        case ACTION_WALK_DOWN:       return &DEF_WALK_DOWN;
        case ACTION_WALK_UP:         return &DEF_WALK_UP;
        case ACTION_WALK_RIGHT_DOWN: return &DEF_WALK_RIGHT_DOWN;
        case ACTION_WALK_LEFT_DOWN:  return &DEF_WALK_LEFT_DOWN;
        case ACTION_WALK_RIGHT_UP:   return &DEF_WALK_RIGHT_UP;
        case ACTION_WALK_LEFT_UP:    return &DEF_WALK_LEFT_UP;
        case ACTION_SLEEP_1_LEFT:    return &DEF_SLEEP_1_LEFT;
        case ACTION_SLEEP_1_RIGHT:   return &DEF_SLEEP_1_RIGHT;
        case ACTION_SLEEP_2_LEFT:    return &DEF_SLEEP_2_LEFT;
        case ACTION_SLEEP_2_RIGHT:   return &DEF_SLEEP_2_RIGHT;
        case ACTION_SLEEP_3_LEFT:    return &DEF_SLEEP_3_LEFT;
        case ACTION_SLEEP_3_RIGHT:   return &DEF_SLEEP_3_RIGHT;
        case ACTION_SLEEP_4_LEFT:    return &DEF_SLEEP_4_LEFT;
        case ACTION_SLEEP_4_RIGHT:   return &DEF_SLEEP_4_RIGHT;
        default:                     return nullptr;
    }
}
