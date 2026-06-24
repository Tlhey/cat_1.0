#include "cat_actor.h"
#include "../assets/cat_action_defs.h"

void CatActor::begin() {
    _posX         = CAT_INIT_X;
    _posY         = CAT_INIT_Y;
    _action       = nullptr;
    _frameIdx     = 0;
    _cyclesPlayed = 0;
    _lastFrameMs  = 0;
    _dirtyFrame   = true;
    _dirtyPos     = false;
}

void CatActor::playAction(CatActionId id, bool restart) {
    const CatActionDef* def = getCatActionDef(id);
    if (!def) return;
    if (!restart && _action == def) return;

    _action       = def;
    _frameIdx     = 0;
    _cyclesPlayed = 0;
    _lastFrameMs  = 0;   // force immediate first frame on next update()
    _dirtyFrame   = true;
}

void CatActor::update(uint32_t nowMs) {
    if (!_action || _action->frameCount == 0) return;

    // If maxCycles is set and we've hit it, freeze on last frame.
    if (_action->maxCycles > 0 && _cyclesPlayed >= _action->maxCycles) return;

    // First call after playAction: seed the timer.
    if (_lastFrameMs == 0) {
        _lastFrameMs = nowMs;
        _dirtyFrame  = true;
        return;
    }

    uint32_t dur = _action->frameDurationMs;
    if ((uint32_t)(nowMs - _lastFrameMs) < dur) return;

    _lastFrameMs = nowMs;

    // Advance frame
    uint8_t nextFrame = _frameIdx + 1;
    if (nextFrame >= _action->frameCount) {
        nextFrame = 0;
        _cyclesPlayed++;
    }

    // Apply movement if still playing (not finished after this cycle)
    if (_action->movesCat) {
        bool willFinish = (_action->maxCycles > 0 && _cyclesPlayed >= _action->maxCycles);
        if (!willFinish) {
            int16_t nx = _posX + _action->moveDx;
            int16_t ny = _posY + _action->moveDy;
            // Clamp to bounds
            if (nx < CAT_MIN_X) nx = CAT_MIN_X;
            if (nx > CAT_MAX_X) nx = CAT_MAX_X;
            if (ny < CAT_MIN_Y) ny = CAT_MIN_Y;
            if (ny > CAT_MAX_Y) ny = CAT_MAX_Y;
            if (nx != _posX || ny != _posY) {
                _posX    = nx;
                _posY    = ny;
                _dirtyPos = true;
            }
        }
    }

    _frameIdx   = nextFrame;
    _dirtyFrame = true;
}

const CatFrame* CatActor::currentFrame() const {
    if (!_action || _action->frameCount == 0) return nullptr;
    const CatFrameRef& ref = _action->frames[_frameIdx];
    return ref.frame;
}

void CatActor::clearDirty() {
    _dirtyFrame = false;
    _dirtyPos   = false;
}

bool CatActor::actionFinished() const {
    if (!_action) return true;
    if (_action->maxCycles == 0) return false;   // infinite loop
    return _cyclesPlayed >= _action->maxCycles;
}

CatActionId CatActor::currentActionId() const {
    return _action ? _action->id : ACTION_NONE;
}

CatActionCategory CatActor::currentCategory() const {
    return _action ? _action->category : CAT_ACTION_REST;
}

CatEmotion CatActor::currentEmotion() const {
    return _action ? _action->defaultEmotion : EMOTION_NEUTRAL;
}
