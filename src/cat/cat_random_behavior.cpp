#include "cat_random_behavior.h"

// Pool of random-idle walk actions (8 directions)
static const CatActionId WALK_POOL[] = {
    ACTION_WALK_RIGHT,
    ACTION_WALK_LEFT,
    ACTION_WALK_DOWN,
    ACTION_WALK_UP,
    ACTION_WALK_RIGHT_DOWN,
    ACTION_WALK_LEFT_DOWN,
    ACTION_WALK_RIGHT_UP,
    ACTION_WALK_LEFT_UP,
};
static const uint8_t WALK_POOL_SIZE = sizeof(WALK_POOL) / sizeof(WALK_POOL[0]);

// Pool of random-idle sleep actions (8 poses)
static const CatActionId SLEEP_POOL[] = {
    ACTION_SLEEP_1_LEFT,
    ACTION_SLEEP_1_RIGHT,
    ACTION_SLEEP_2_LEFT,
    ACTION_SLEEP_2_RIGHT,
    ACTION_SLEEP_3_LEFT,
    ACTION_SLEEP_3_RIGHT,
    ACTION_SLEEP_4_LEFT,
    ACTION_SLEEP_4_RIGHT,
};
static const uint8_t SLEEP_POOL_SIZE = sizeof(SLEEP_POOL) / sizeof(SLEEP_POOL[0]);

// Cooldown range: 1500–4000 ms
static const uint32_t COOLDOWN_MIN_MS = 1500;
static const uint32_t COOLDOWN_MAX_MS = 4000;

// Walk actions that move the cat in a given dx direction (positive = right)
static inline bool movesRight(CatActionId id) {
    return id == ACTION_WALK_RIGHT || id == ACTION_WALK_RIGHT_UP || id == ACTION_WALK_RIGHT_DOWN;
}
static inline bool movesLeft(CatActionId id) {
    return id == ACTION_WALK_LEFT || id == ACTION_WALK_LEFT_UP || id == ACTION_WALK_LEFT_DOWN;
}
static inline bool movesDown(CatActionId id) {
    return id == ACTION_WALK_DOWN || id == ACTION_WALK_RIGHT_DOWN || id == ACTION_WALK_LEFT_DOWN;
}
static inline bool movesUp(CatActionId id) {
    return id == ACTION_WALK_UP || id == ACTION_WALK_RIGHT_UP || id == ACTION_WALK_LEFT_UP;
}

void CatRandomBehavior::begin(uint32_t seed) {
    randomSeed(seed);
    _cooldownEndMs = 0;
    _waiting       = false;
    _firstAction   = true;
}

CatActionId CatRandomBehavior::pickWalkAction(const CatActor& actor) const {
    // Build a filtered pool: exclude directions that would push the cat out of bounds.
    CatActionId filtered[WALK_POOL_SIZE];
    uint8_t count = 0;

    for (uint8_t i = 0; i < WALK_POOL_SIZE; i++) {
        CatActionId id = WALK_POOL[i];
        bool atRight  = actor.x() >= CAT_MAX_X;
        bool atLeft   = actor.x() <= CAT_MIN_X;
        bool atBottom = actor.y() >= CAT_MAX_Y;
        bool atTop    = actor.y() <= CAT_MIN_Y;

        if (atRight  && movesRight(id)) continue;
        if (atLeft   && movesLeft(id))  continue;
        if (atBottom && movesDown(id))  continue;
        if (atTop    && movesUp(id))    continue;

        filtered[count++] = id;
    }

    if (count == 0) return WALK_POOL[random(WALK_POOL_SIZE)]; // fallback (shouldn't happen)
    return filtered[random(count)];
}

CatActionId CatRandomBehavior::pickSleepAction() const {
    return SLEEP_POOL[random(SLEEP_POOL_SIZE)];
}

CatActionId CatRandomBehavior::pickNextAction(const CatActor& actor) const {
    // 70% walk, 30% sleep
    if (random(100) < 70) return pickWalkAction(actor);
    return pickSleepAction();
}

void CatRandomBehavior::update(CatActor& actor, uint32_t nowMs) {
    // Kick off the very first action immediately.
    if (_firstAction) {
        _firstAction = false;
        actor.playAction(pickNextAction(actor));
        return;
    }

    // While an action is running, do nothing.
    if (!actor.actionFinished()) return;

    // Action just finished — start cooldown if not already waiting.
    if (!_waiting) {
        uint32_t delay = COOLDOWN_MIN_MS + (uint32_t)random(COOLDOWN_MAX_MS - COOLDOWN_MIN_MS);
        _cooldownEndMs = nowMs + delay;
        _waiting = true;
    }

    // Wait out the cooldown.
    if ((int32_t)(nowMs - _cooldownEndMs) < 0) return;

    // Cooldown over — pick a new action.
    _waiting = false;
    actor.playAction(pickNextAction(actor));
}
