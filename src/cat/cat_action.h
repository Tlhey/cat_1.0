#pragma once
#include <Arduino.h>
#include "cat_animation.h"   // CatFrameRef

// ── Action taxonomy ───────────────────────────────────────────
enum CatActionCategory : uint8_t {
    CAT_ACTION_REST,
    CAT_ACTION_WALK,
    CAT_ACTION_SLEEP,
    CAT_ACTION_EAT,
    CAT_ACTION_MEOW,
    CAT_ACTION_YAWN,
    CAT_ACTION_WASH,
    CAT_ACTION_ITCH,
    CAT_ACTION_HISS,
    CAT_ACTION_PAW_ATTACK,
    CAT_ACTION_ON_HIND_LEGS
};

enum CatActionId : uint8_t {
    ACTION_NONE,
    // WALK
    ACTION_WALK_DOWN,
    ACTION_WALK_UP,
    ACTION_WALK_RIGHT,
    ACTION_WALK_LEFT,
    ACTION_WALK_RIGHT_UP,
    ACTION_WALK_LEFT_UP,
    ACTION_WALK_RIGHT_DOWN,
    ACTION_WALK_LEFT_DOWN,
    // SLEEP
    ACTION_SLEEP_1_LEFT,
    ACTION_SLEEP_1_RIGHT,
    ACTION_SLEEP_2_LEFT,
    ACTION_SLEEP_2_RIGHT,
    ACTION_SLEEP_3_LEFT,
    ACTION_SLEEP_3_RIGHT,
    ACTION_SLEEP_4_LEFT,
    ACTION_SLEEP_4_RIGHT,
    // EAT (placeholder)
    ACTION_EAT_DOWN, ACTION_EAT_UP,
    ACTION_EAT_LEFT, ACTION_EAT_RIGHT,
    ACTION_EAT_RIGHT_DOWN, ACTION_EAT_LEFT_DOWN,
    ACTION_EAT_RIGHT_UP,   ACTION_EAT_LEFT_UP,
    // MEOW (placeholder)
    ACTION_MEOW_SIT, ACTION_MEOW_STAND,
    ACTION_MEOW_SIT_2, ACTION_MEOW_LIE,
    // YAWN (placeholder)
    ACTION_YAWN_SIT, ACTION_YAWN_STAND,
    ACTION_YAWN_SIT_2, ACTION_YAWN_LIE,
    // WASH (placeholder)
    ACTION_WASH_SIT, ACTION_WASH_STAND, ACTION_WASH_LIE,
    // ITCH (placeholder)
    ACTION_SCRATCH_LEFT, ACTION_SCRATCH_RIGHT,
    // HISS (placeholder)
    ACTION_HISS_LEFT, ACTION_HISS_RIGHT,
    // PAW ATTACK (placeholder)
    ACTION_PAW_ATTACK_DOWN, ACTION_PAW_ATTACK_UP,
    ACTION_PAW_ATTACK_LEFT, ACTION_PAW_ATTACK_RIGHT,
    ACTION_PAW_ATTACK_RIGHT_DOWN, ACTION_PAW_ATTACK_LEFT_DOWN,
    ACTION_PAW_ATTACK_RIGHT_UP,   ACTION_PAW_ATTACK_LEFT_UP,
    // ON HIND LEGS (placeholder)
    ACTION_ON_HIND_LEGS
};

enum CatEmotion : uint8_t {
    EMOTION_NEUTRAL,
    EMOTION_CURIOUS,
    EMOTION_SLEEPY,
    EMOTION_RELAXED,
    EMOTION_HUNGRY,
    EMOTION_HAPPY,
    EMOTION_BORED,
    EMOTION_ANNOYED,
    EMOTION_ANGRY,
    EMOTION_PLAYFUL,
    EMOTION_SURPRISED
};

// ── Per-action definition ──────────────────────────────────────
struct CatActionDef {
    CatActionId         id;
    CatActionCategory   category;
    CatEmotion          defaultEmotion;

    const CatFrameRef*  frames;
    uint8_t             frameCount;

    bool                canBeRandomIdle;
    bool                movesCat;

    int8_t              moveDx;      // pixels per frame advance
    int8_t              moveDy;

    uint16_t            frameDurationMs;
    uint8_t             maxCycles;   // 0=infinite; >0=stop after N full loops
};

// ── Name helpers (cat_action.cpp) ─────────────────────────────
const char* catActionName(CatActionId id);
const char* catCategoryName(CatActionCategory cat);
const char* catEmotionName(CatEmotion emotion);
