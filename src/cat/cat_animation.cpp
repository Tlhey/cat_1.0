#include "cat_animation.h"
#include "../assets/cat_anim_defs.h"

void CatAnimationPlayer::begin() {
    _anim     = nullptr;
    _frameIdx = 0;
    _lastMs   = 0;
    _changed  = true;
}

void CatAnimationPlayer::play(CatAnimId id, bool restart) {
    const CatAnimDef* def = getCatAnimDef(id);
    if (def == _anim && !restart) return;
    _anim     = def;
    _frameIdx = 0;
    _lastMs   = millis();
    _changed  = true;
}

void CatAnimationPlayer::update(uint32_t nowMs) {
    if (!_anim || _anim->frameCount == 0) return;

    uint16_t dur = _anim->frames[_frameIdx].durationMs;
    if ((uint32_t)(nowMs - _lastMs) >= dur) {
        uint8_t next = _frameIdx + 1;
        if (next >= _anim->frameCount) {
            next = _anim->loop ? 0 : _frameIdx;
        }
        if (next != _frameIdx) {
            _frameIdx = next;
            _lastMs   = nowMs;
            _changed  = true;
        } else if (!_anim->loop) {
            // last frame of non-loop anim; reset timer so we don't spin
            _lastMs = nowMs;
        }
    }
}

const CatFrame* CatAnimationPlayer::currentFrame() const {
    if (!_anim || _anim->frameCount == 0) return nullptr;
    return _anim->frames[_frameIdx].frame;
}

bool CatAnimationPlayer::frameChanged() const { return _changed; }
void CatAnimationPlayer::clearFrameChanged()  { _changed = false; }
