#pragma once
#include <Arduino.h>
#include "../assets/cat_seqs.h"
#include "../board/board_config.h"

// ── Direction enum ──────────────────────────────────────────────────────────
// Indices must match WALK_SEQ / EAT_SEQ / PAW_SEQ lookup tables in cat_seqs.h
enum CatDir : uint8_t {
    DIR_DOWN  = 0, DIR_UP    = 1, DIR_RIGHT = 2, DIR_LEFT = 3,
    DIR_DL    = 4, DIR_DR    = 5, DIR_UR    = 6, DIR_UL   = 7
};

// ── Behavior tags (finish-transition logic) ─────────────────────────────────
enum CatBehTag : uint8_t {
    TAG_SLOW_PATROL,
    TAG_FAST_RUN,
    TAG_WALK_CHANGE,
    TAG_DEEP_SLEEP,
    TAG_SLEEP_FAST,    // purring fast mid-sleep
    TAG_PURR_TOUCHED,
    TAG_WAKE_YAWN,
    TAG_RUN_TO,        // running toward a touch target
    TAG_ARRIVE,        // arrived at touch target
    TAG_ATTACK,
    TAG_WASH_AFTER_ATTACK,
    TAG_RUN_AWAY,
    TAG_GENERIC,
};

enum CatMoveMode : uint8_t { MOVE_STILL, MOVE_DIR, MOVE_TARGET, MOVE_AWAY };

// ── Behavior descriptor ─────────────────────────────────────────────────────
struct CatBeh {
    CatBehTag   tag;
    CatMoveMode moveMode;
    CatDir      dir;         // used by MOVE_DIR; updated when MOVE_TARGET changes heading
    const CatSeq* seq;
    uint16_t    intervalMs;
    float       speedPxMs;  // px per ms (0 = stationary)
    float       targetX, targetY;
    uint32_t    durationMs; // 0 = no time limit
    uint8_t     maxLoops;   // 0 = no loop limit
};

// ── Screen bounds ────────────────────────────────────────────────────────────
#define PET_CAT_PX   (CAT_FRAME_W * CAT_SCALE)             // 192
#define PET_MIN_X    0.0f
#define PET_MAX_X    ((float)(SCREEN_W - PET_CAT_PX))      // 176
#define PET_MIN_Y    0.0f
#define PET_MAX_Y    ((float)(SCREEN_H - PET_CAT_PX))      // 256

// ── CatPet ──────────────────────────────────────────────────────────────────
class CatPet {
public:
    void begin(uint32_t seed = 0);
    void update(uint32_t nowMs);
    void onTouch(int16_t tx, int16_t ty, uint32_t now);

    const CatFrame* currentFrame() const;
    int16_t x()          const { return (int16_t)_x; }
    int16_t y()          const { return (int16_t)_y; }
    bool    needsRedraw()const { return _dirty; }
    void    clearDirty()       { _dirty = false; }

private:
    // Position (float for smooth movement)
    float _x = 0, _y = 0;

    // Current behavior
    CatBeh   _beh{};
    uint8_t  _fi = 0;
    uint8_t  _loops = 0;
    uint32_t _lastFrameMs = 0;
    uint32_t _behStartMs  = 0;
    uint32_t _lastUpdateMs = 0;

    // Auto-switch timing
    uint32_t _lastAutoMs  = 0;
    uint32_t _nextAutoMs  = 0;

    // Sleep memory (which pose)
    uint8_t  _sleepPose = 0;

    // Walking direction (for purr direction tracking)
    CatDir   _dir = DIR_DOWN;

    bool _dirty = true;

    // ── Touch state ────────────────────────────────────────────────────────
    static const uint8_t MAX_TOUCH = 8;
    uint32_t _touchTimes[MAX_TOUCH];
    uint8_t  _touchCount = 0;
    float    _lastTouchX = 0, _lastTouchY = 0;

    // ── Core ───────────────────────────────────────────────────────────────
    void setBeh(CatBeh b, uint32_t now);
    void updateAnim(uint32_t now);
    void updateMove(float dtMs);
    void checkFinish(uint32_t now);
    void finishBeh(uint32_t now);
    void maybeAuto(uint32_t now);
    void bounce();

    // ── Direction helpers ──────────────────────────────────────────────────
    static CatDir vecToDir(float dx, float dy);
    static CatDir opposite(CatDir d);
    static void   dirVec(CatDir d, float& vx, float& vy);

    // ── High-level behaviors ───────────────────────────────────────────────
    void slowPatrol(uint32_t now);
    void fastRun(uint32_t now);
    void changeDirWalk(uint32_t now);
    void deepSleep(uint32_t now);
    void resumeDeepSleep(uint32_t now);
    void wakeYawn(uint32_t now);
    void purrTouched(uint32_t now);
    void afterPurr(uint32_t now);
    void randomAction(uint32_t now);
    void washAfterAttack(uint32_t now);
    void genericAction(CatBehTag tag, const CatSeq* seq,
                       uint16_t ms, uint8_t loops, uint32_t now);
    void runTo(float tx, float ty, uint32_t now);
    void arriveAtTarget(uint32_t now);
    void purrFastSleep(uint32_t now);

    // ── Touch dispatch ─────────────────────────────────────────────────────
    void pruneTouchTimes(uint32_t now);
    void handleCatTouch(uint32_t now);
    void handleWorldTouch(float tx, float ty, uint32_t now);

    // ── Random action weighted selection ──────────────────────────────────
    static const CatSeq* pickActionSeq(uint16_t& ms, uint8_t& loops,
                                        CatBehTag& tag, bool& isSleep);
};
