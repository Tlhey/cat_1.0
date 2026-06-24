#pragma once
#include "../cat/cat_animation.h"
#include "../cat/cat_actor.h"

void rendererBegin();
void rendererDrawStaticUI();

// Phase 1 (kept for compatibility)
void rendererDrawCat(const CatFrame* frame);
void rendererUpdate(CatAnimationPlayer& anim);

// Phase 2
void rendererDrawCatActor(CatActor& actor);
