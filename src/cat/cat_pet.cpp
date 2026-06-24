#include "cat_pet.h"
#include <math.h>

// ── Direction vectors (screen coords: x→right, y→down) ─────────────────────
static const float DVX[8] = { 0,  0, 1, -1, -0.7071f,  0.7071f,  0.7071f, -0.7071f };
static const float DVY[8] = { 1, -1, 0,  0,  0.7071f,  0.7071f, -0.7071f, -0.7071f };

// Opposite direction table
static const CatDir OPP[8] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT,
                                DIR_UR, DIR_UL,   DIR_DL,   DIR_DR };

// ── Weighted action table ───────────────────────────────────────────────────
// {seq*, interval_ms, loops, tag, weight}
struct ActEntry {
    const CatSeq* seq;
    uint16_t ms; uint8_t loops;
    CatBehTag tag;
    bool isSleep;
    uint8_t w;
};

static const ActEntry ACT_TABLE[] = {
    // REST
    { &SEQ_REST_1, 140, 5, TAG_GENERIC, false, 10 },
    { &SEQ_REST_2, 140, 5, TAG_GENERIC, false, 10 },
    { &SEQ_REST_3, 140, 5, TAG_GENERIC, false, 10 },
    { &SEQ_REST_4, 140, 5, TAG_GENERIC, false, 10 },
    // SLEEP (routes to deepSleep)
    { nullptr,     0,   0, TAG_DEEP_SLEEP, true, 8 },
    // EAT (8 dirs)
    { &SEQ_EAT_DOWN,       120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_UP,         120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_LEFT,       120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_RIGHT,      120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_DOWN_RIGHT, 120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_DOWN_LEFT,  120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_UP_RIGHT,   120, 5, TAG_GENERIC, false, 8 },
    { &SEQ_EAT_UP_LEFT,    120, 5, TAG_GENERIC, false, 8 },
    // MEOW
    { &SEQ_MEOW_SIT,   130, 4, TAG_GENERIC, false, 10 },
    { &SEQ_MEOW_STAND, 130, 4, TAG_GENERIC, false, 10 },
    { &SEQ_MEOW_SIT_2, 130, 4, TAG_GENERIC, false, 10 },
    { &SEQ_MEOW_LIE,   130, 4, TAG_GENERIC, false, 10 },
    // YAWN
    { &SEQ_YAWN_SIT,   190, 2, TAG_GENERIC, false, 9 },
    { &SEQ_YAWN_STAND, 190, 2, TAG_GENERIC, false, 9 },
    { &SEQ_YAWN_SIT_2, 190, 2, TAG_GENERIC, false, 9 },
    { &SEQ_YAWN_LIE,   190, 2, TAG_GENERIC, false, 9 },
    // WASH
    { &SEQ_WASH_SIT,   150, 5, TAG_GENERIC, false, 13 },
    { &SEQ_WASH_STAND, 150, 5, TAG_GENERIC, false, 13 },
    { &SEQ_WASH_LIE,   150, 5, TAG_GENERIC, false, 13 },
    // SCRATCH
    { &SEQ_SCRATCH_L, 100, 3, TAG_GENERIC, false, 8 },
    { &SEQ_SCRATCH_R, 100, 3, TAG_GENERIC, false, 8 },
    // HISS
    { &SEQ_HISS_L, 100, 2, TAG_GENERIC, false, 4 },
    { &SEQ_HISS_R, 100, 2, TAG_GENERIC, false, 4 },
    // PAW ATTACK
    { &SEQ_PAW_DOWN,       75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_UP,         75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_LEFT,       75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_RIGHT,      75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_DOWN_RIGHT, 75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_DOWN_LEFT,  75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_UP_RIGHT,   75, 3, TAG_ATTACK, false, 5 },
    { &SEQ_PAW_UP_LEFT,    75, 3, TAG_ATTACK, false, 5 },
    // ON HIND LEGS
    { &SEQ_HIND, 140, 3, TAG_GENERIC, false, 8 },
};

static const uint8_t ACT_TABLE_N = sizeof(ACT_TABLE) / sizeof(ACT_TABLE[0]);

// ─────────────────────────────────────────────────────────────────────────────

void CatPet::begin(uint32_t seed) {
    randomSeed(seed);

    _x = PET_MIN_X + (PET_MAX_X - PET_MIN_X) * 0.5f;
    _y = PET_MIN_Y + (PET_MAX_Y - PET_MIN_Y) * 0.5f;
    _dir = (CatDir)random(8);
    _sleepPose = (uint8_t)random(8);
    _dirty = true;
    _touchCount = 0;

    uint32_t now = millis();
    _lastUpdateMs = now;
    _lastAutoMs   = now;
    _nextAutoMs   = (uint32_t)random(3000, 7000); // don't fire immediately
    slowPatrol(now);
}

void CatPet::update(uint32_t nowMs) {
    float dt = (float)(nowMs - _lastUpdateMs);
    _lastUpdateMs = nowMs;
    if (dt > 200) dt = 200; // clamp on first frame or freeze

    updateAnim(nowMs);
    updateMove(dt);
    checkFinish(nowMs);
}

const CatFrame* CatPet::currentFrame() const {
    if (!_beh.seq || _beh.seq->n == 0) return nullptr;
    return _beh.seq->f[_fi % _beh.seq->n];
}

// ── Core ─────────────────────────────────────────────────────────────────────
void CatPet::setBeh(CatBeh b, uint32_t now) {
    _beh = b;
    _fi = 0;
    _loops = 0;
    _lastFrameMs = now;
    _behStartMs  = now;
    _dirty = true;
}

void CatPet::updateAnim(uint32_t now) {
    if (!_beh.seq || _beh.seq->n == 0) return;
    if ((uint32_t)(now - _lastFrameMs) < _beh.intervalMs) return;

    _lastFrameMs = now;
    uint8_t prev = _fi;
    _fi = (_fi + 1) % _beh.seq->n;
    if (_fi == 0 && prev != 0) _loops++;
    _dirty = true;
}

void CatPet::updateMove(float dtMs) {
    if (_beh.moveMode == MOVE_STILL || _beh.speedPxMs <= 0) return;

    float vx = 0, vy = 0;

    if (_beh.moveMode == MOVE_DIR) {
        dirVec(_beh.dir, vx, vy);

    } else if (_beh.moveMode == MOVE_TARGET) {
        float cx = _x + PET_CAT_PX * 0.5f;
        float cy = _y + PET_CAT_PX * 0.5f;
        float dx = _beh.targetX - cx;
        float dy = _beh.targetY - cy;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < (_beh.speedPxMs * 16 + 2)) return; // arrived
        vx = dx / dist;
        vy = dy / dist;
        // Update heading
        CatDir nd = vecToDir(dx, dy);
        if (nd != _beh.dir) {
            _beh.dir = nd;
            _dir = nd;
            _fi = 0;
            for (uint8_t i = 0; i < 8; i++) {
                if (_beh.seq == WALK_SEQ[i]) { _beh.seq = WALK_SEQ[nd]; break; }
            }
        }

    } else if (_beh.moveMode == MOVE_AWAY) {
        float cx = _x + PET_CAT_PX * 0.5f;
        float cy = _y + PET_CAT_PX * 0.5f;
        float dx = cx - _beh.targetX;
        float dy = cy - _beh.targetY;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < 1) { dx = 1; dy = 0; dist = 1; }
        vx = dx / dist;
        vy = dy / dist;
    }

    _x += vx * _beh.speedPxMs * dtMs;
    _y += vy * _beh.speedPxMs * dtMs;
    bounce();
    _dirty = true;
}

void CatPet::bounce() {
    bool bounced = false;
    if (_x < PET_MIN_X) { _x = PET_MIN_X; bounced = true; }
    if (_x > PET_MAX_X) { _x = PET_MAX_X; bounced = true; }
    if (_y < PET_MIN_Y) { _y = PET_MIN_Y; bounced = true; }
    if (_y > PET_MAX_Y) { _y = PET_MAX_Y; bounced = true; }

    if (bounced && _beh.moveMode == MOVE_DIR) {
        CatDir nd = opposite(_beh.dir);
        _beh.dir = nd;
        _dir = nd;
        _beh.seq = WALK_SEQ[nd];
        _fi = 0;
    }
}

void CatPet::checkFinish(uint32_t now) {
    bool durationDone = _beh.durationMs > 0 &&
                        (uint32_t)(now - _behStartMs) >= _beh.durationMs;
    bool loopsDone    = _beh.maxLoops > 0 && _loops >= _beh.maxLoops;

    bool targetDone = false;
    if (_beh.moveMode == MOVE_TARGET) {
        float cx = _x + PET_CAT_PX * 0.5f;
        float cy = _y + PET_CAT_PX * 0.5f;
        float dx = _beh.targetX - cx;
        float dy = _beh.targetY - cy;
        targetDone = (dx*dx + dy*dy) < (14*14);
    }

    bool finished = durationDone || loopsDone || targetDone;

    if (!finished) {
        if (_beh.moveMode == MOVE_DIR) maybeAuto(now); // free-roam only
        return;
    }
    finishBeh(now);
}

void CatPet::finishBeh(uint32_t now) {
    switch (_beh.tag) {
        case TAG_DEEP_SLEEP:        wakeYawn(now);        break;
        case TAG_SLEEP_FAST:        resumeDeepSleep(now); break;
        case TAG_WAKE_YAWN:         slowPatrol(now);      break;
        case TAG_ATTACK:            washAfterAttack(now); break;
        case TAG_WASH_AFTER_ATTACK: slowPatrol(now);      break;
        case TAG_RUN_AWAY:          slowPatrol(now);      break;
        case TAG_RUN_TO:            arriveAtTarget(now);  break;
        case TAG_ARRIVE:            slowPatrol(now);      break;
        default:                    slowPatrol(now);      break;
    }
}

void CatPet::maybeAuto(uint32_t now) {
    if ((uint32_t)(now - _lastAutoMs) < _nextAutoMs) return;
    _lastAutoMs = now;
    _nextAutoMs = (uint32_t)random(2500, 7000);

    int roll = (int)random(100);
    if      (roll < 20) randomAction(now);
    else if (roll < 36) slowPatrol(now);
    else if (roll < 45) fastRun(now);
    else if (roll < 52) deepSleep(now);
    else                changeDirWalk(now);
}

// ── Direction helpers ────────────────────────────────────────────────────────
CatDir CatPet::vecToDir(float dx, float dy) {
    float angle = atan2f(dy, dx) * 57.2957795f;
    if (angle >= -22.5f && angle < 22.5f)   return DIR_RIGHT;
    if (angle >= 22.5f  && angle < 67.5f)   return DIR_DR;
    if (angle >= 67.5f  && angle < 112.5f)  return DIR_DOWN;
    if (angle >= 112.5f && angle < 157.5f)  return DIR_DL;
    if (angle >= 157.5f || angle < -157.5f) return DIR_LEFT;
    if (angle >= -157.5f&& angle < -112.5f) return DIR_UL;
    if (angle >= -112.5f&& angle < -67.5f)  return DIR_UP;
    return DIR_UR;
}

CatDir CatPet::opposite(CatDir d) { return OPP[d]; }

void CatPet::dirVec(CatDir d, float& vx, float& vy) {
    vx = DVX[d]; vy = DVY[d];
}

// ── High-level behaviors ─────────────────────────────────────────────────────
void CatPet::slowPatrol(uint32_t now) {
    CatDir d = (CatDir)random(8);
    CatBeh b{};
    b.tag        = TAG_SLOW_PATROL;
    b.moveMode   = MOVE_DIR;
    b.dir        = d;
    b.seq        = WALK_SEQ[d];
    b.intervalMs = (uint16_t)random(180, 260);
    b.speedPxMs  = 0.030f + random(0, 27) * 0.001f; // 0.030–0.057 px/ms
    b.durationMs = (uint32_t)random(3000, 7000);
    _dir = d;
    setBeh(b, now);
}

void CatPet::fastRun(uint32_t now) {
    CatDir d = (CatDir)random(8);
    CatBeh b{};
    b.tag        = TAG_FAST_RUN;
    b.moveMode   = MOVE_DIR;
    b.dir        = d;
    b.seq        = WALK_SEQ[d];
    b.intervalMs = (uint16_t)random(55, 85);
    b.speedPxMs  = 0.100f + random(0, 67) * 0.001f; // 0.100–0.167 px/ms
    b.durationMs = (uint32_t)random(1500, 3500);
    _dir = d;
    setBeh(b, now);
}

void CatPet::changeDirWalk(uint32_t now) {
    CatDir d;
    do { d = (CatDir)random(8); } while (d == _dir);
    CatBeh b{};
    b.tag        = TAG_WALK_CHANGE;
    b.moveMode   = MOVE_DIR;
    b.dir        = d;
    b.seq        = WALK_SEQ[d];
    b.intervalMs = (uint16_t)random(120, 200);
    b.speedPxMs  = 0.040f + random(0, 47) * 0.001f;
    b.durationMs = (uint32_t)random(2500, 5500);
    _dir = d;
    setBeh(b, now);
}

void CatPet::deepSleep(uint32_t now) {
    _sleepPose = (uint8_t)random(8);
    CatBeh b{};
    b.tag        = TAG_DEEP_SLEEP;
    b.moveMode   = MOVE_STILL;
    b.seq        = SLEEP_SEQ[_sleepPose];
    b.intervalMs = 3000;
    b.durationMs = 60000;
    setBeh(b, now);
}

void CatPet::resumeDeepSleep(uint32_t now) {
    CatBeh b{};
    b.tag        = TAG_DEEP_SLEEP;
    b.moveMode   = MOVE_STILL;
    b.seq        = SLEEP_SEQ[_sleepPose];
    b.intervalMs = 3000;
    b.durationMs = 60000;
    setBeh(b, now);
}

void CatPet::wakeYawn(uint32_t now) {
    const CatSeq* yawns[3] = { &SEQ_YAWN_LIE, &SEQ_YAWN_SIT, &SEQ_YAWN_STAND };
    genericAction(TAG_WAKE_YAWN, yawns[random(3)], 190, 2, now);
}

void CatPet::purrTouched(uint32_t now) {
    genericAction(TAG_PURR_TOUCHED, EAT_SEQ[_dir], 110, 0, now);
    _beh.durationMs = 5000;
}

void CatPet::afterPurr(uint32_t now) {
    int roll = (int)random(100);
    if      (roll < 35) genericAction(TAG_GENERIC, (random(2) ? &SEQ_MEOW_SIT : &SEQ_MEOW_STAND), 130, 3, now);
    else if (roll < 65) genericAction(TAG_GENERIC, (random(2) ? &SEQ_WASH_SIT : &SEQ_WASH_STAND), 150, 5, now);
    else if (roll < 80) deepSleep(now);
    else                slowPatrol(now);
}

void CatPet::washAfterAttack(uint32_t now) {
    genericAction(TAG_WASH_AFTER_ATTACK,
                  random(2) ? &SEQ_WASH_SIT : &SEQ_WASH_STAND,
                  150, 5, now);
}

void CatPet::genericAction(CatBehTag tag, const CatSeq* seq,
                            uint16_t ms, uint8_t loops, uint32_t now) {
    CatBeh b{};
    b.tag        = tag;
    b.moveMode   = MOVE_STILL;
    b.seq        = seq;
    b.intervalMs = ms;
    b.maxLoops   = loops;
    setBeh(b, now);
}

void CatPet::randomAction(uint32_t now) {
    uint16_t ms; uint8_t loops; CatBehTag tag; bool isSleep;
    const CatSeq* seq = pickActionSeq(ms, loops, tag, isSleep);
    if (isSleep) { deepSleep(now); return; }
    genericAction(tag, seq, ms, loops, now);
}

// ── Weighted random action ────────────────────────────────────────────────────
const CatSeq* CatPet::pickActionSeq(uint16_t& ms, uint8_t& loops,
                                     CatBehTag& tag, bool& isSleep) {
    uint16_t total = 0;
    for (uint8_t i = 0; i < ACT_TABLE_N; i++) total += ACT_TABLE[i].w;

    uint16_t r = (uint16_t)random(total);
    uint16_t acc = 0;
    for (uint8_t i = 0; i < ACT_TABLE_N; i++) {
        acc += ACT_TABLE[i].w;
        if (r < acc) {
            ms     = ACT_TABLE[i].ms;
            loops  = ACT_TABLE[i].loops;
            tag    = ACT_TABLE[i].tag;
            isSleep= ACT_TABLE[i].isSleep;
            return ACT_TABLE[i].seq;
        }
    }
    ms = 150; loops = 5; tag = TAG_GENERIC; isSleep = false;
    return &SEQ_WASH_SIT;
}

// ── Touch behaviors ──────────────────────────────────────────────────────────
void CatPet::runTo(float tx, float ty, uint32_t now) {
    float cx = _x + PET_CAT_PX * 0.5f;
    float cy = _y + PET_CAT_PX * 0.5f;
    CatDir d = vecToDir(tx - cx, ty - cy);
    CatBeh b{};
    b.tag        = TAG_RUN_TO;
    b.moveMode   = MOVE_TARGET;
    b.dir        = d;
    b.seq        = WALK_SEQ[d];
    b.intervalMs = (uint16_t)random(55, 80);
    b.speedPxMs  = 0.117f + random(0, 66) * 0.001f; // ~7–11 px/frame@60fps
    b.targetX    = tx;
    b.targetY    = ty;
    b.durationMs = 8000;
    _dir = d;
    setBeh(b, now);
}

void CatPet::arriveAtTarget(uint32_t now) {
    int roll = (int)random(100);
    if      (roll < 45) genericAction(TAG_ARRIVE, &SEQ_MEOW_STAND, 130, 3, now);
    else if (roll < 75) genericAction(TAG_ARRIVE, &SEQ_HIND,       140, 3, now);
    else                genericAction(TAG_ARRIVE,
                            random(2) ? &SEQ_WASH_SIT : &SEQ_WASH_STAND, 150, 5, now);
}

// Faster sleep breathing when touched mid-sleep
void CatPet::purrFastSleep(uint32_t now) {
    CatBeh b{};
    b.tag        = TAG_SLEEP_FAST;
    b.moveMode   = MOVE_STILL;
    b.seq        = SLEEP_SEQ[_sleepPose];
    b.intervalMs = 800;
    b.durationMs = 4000;
    setBeh(b, now);
}

// ── Touch dispatch ────────────────────────────────────────────────────────────
void CatPet::pruneTouchTimes(uint32_t now) {
    uint8_t n = 0;
    for (uint8_t i = 0; i < _touchCount; i++) {
        if ((uint32_t)(now - _touchTimes[i]) <= 10000)
            _touchTimes[n++] = _touchTimes[i];
    }
    _touchCount = n;
}

void CatPet::handleCatTouch(uint32_t now) {
    pruneTouchTimes(now);
    if (_touchCount < MAX_TOUCH) _touchTimes[_touchCount++] = now;

    // 6+ taps in 10s → hiss and run away
    if (_touchCount >= 6) {
        _touchCount = 0;
        const CatSeq* hiss = random(2) ? &SEQ_HISS_L : &SEQ_HISS_R;
        genericAction(TAG_RUN_AWAY, hiss, 100, 2, now);
        return;
    }

    // Mid-sleep touch
    if (_beh.tag == TAG_DEEP_SLEEP || _beh.tag == TAG_SLEEP_FAST) {
        if (_touchCount >= 3) {
            _touchCount = 0;
            const CatSeq* wake = random(2) ? &SEQ_YAWN_LIE : &SEQ_MEOW_LIE;
            genericAction(TAG_WAKE_YAWN, wake, 170, 2, now);
        } else {
            purrFastSleep(now);
        }
        return;
    }

    // Default: purr (75%) or attack (25%)
    if (random(100) < 75) {
        purrTouched(now);
    } else {
        // Random paw attack toward last touch
        float cx = _x + PET_CAT_PX * 0.5f;
        float cy = _y + PET_CAT_PX * 0.5f;
        CatDir d = vecToDir(_lastTouchX - cx, _lastTouchY - cy);
        genericAction(TAG_ATTACK, PAW_SEQ[d], 75, 3, now);
    }
}

void CatPet::handleWorldTouch(float tx, float ty, uint32_t now) {
    // Sleeping: mostly ignore, sometimes meow
    if (_beh.tag == TAG_DEEP_SLEEP || _beh.tag == TAG_SLEEP_FAST) {
        if (random(100) < 70) {
            purrFastSleep(now);
        } else {
            genericAction(TAG_GENERIC, &SEQ_MEOW_LIE, 130, 3, now);
        }
        return;
    }
    // 15% chance to do a stationary action instead of running
    if (random(100) < 15) {
        int r = (int)random(3);
        const CatSeq* s = (r == 0) ? &SEQ_WASH_SIT
                        : (r == 1) ? &SEQ_WASH_STAND
                        :            &SEQ_MEOW_STAND;
        genericAction(TAG_GENERIC, s, 150, 4, now);
        return;
    }
    runTo(tx, ty, now);
}

void CatPet::onTouch(int16_t tx, int16_t ty, uint32_t now) {
    _lastTouchX = (float)tx;
    _lastTouchY = (float)ty;

    bool onCat = (tx >= (int16_t)_x && tx < (int16_t)_x + (int16_t)PET_CAT_PX &&
                  ty >= (int16_t)_y && ty < (int16_t)_y + (int16_t)PET_CAT_PX);

    if (onCat) handleCatTouch(now);
    else        handleWorldTouch((float)tx, (float)ty, now);
}
