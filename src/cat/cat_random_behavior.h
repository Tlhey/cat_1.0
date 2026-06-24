#pragma once
#include <Arduino.h>
#include "cat_action.h"
#include "cat_actor.h"

// Drives idle random behaviour: 70% WALK, 30% SLEEP.
// Walk direction is chosen to keep the cat on-screen (won't walk into a wall).
// REST is never chosen. Waits a random 1.5–4 s between actions.
class CatRandomBehavior {
public:
    void begin(uint32_t seed = 0);
    void update(CatActor& actor, uint32_t nowMs);

private:
    uint32_t _cooldownEndMs = 0;
    bool     _waiting       = false;
    bool     _firstAction   = true;

    CatActionId pickWalkAction(const CatActor& actor) const;
    CatActionId pickSleepAction() const;
    CatActionId pickNextAction(const CatActor& actor) const;
};
