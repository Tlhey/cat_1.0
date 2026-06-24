#include "app_loop.h"
#include "../board/board_display.h"
#include "../render/renderer.h"
#include "../cat/cat_animation.h"
#include "../assets/cat_anim_defs.h"

static CatAnimationPlayer catAnim;
static uint8_t            _logCount = 0;

void appSetup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("CatPet Phase 1 starting...");

    if (!displayBegin()) {
        Serial.println("Display init FAILED");
        while (true) delay(1000);
    }
    Serial.println("Display init OK");

    rendererBegin();
    Serial.println("Renderer init OK");

    catAnim.begin();
    catAnim.play(ANIM_IDLE_FRONT);
    Serial.println("Animation player init OK");
    Serial.println("Playing ANIM_IDLE_FRONT");

    rendererDrawStaticUI();
    rendererDrawCat(catAnim.currentFrame());
    catAnim.clearFrameChanged();
}

void appLoop() {
    catAnim.update(millis());

    // Log the first 12 frame transitions, then go quiet.
    if (catAnim.frameChanged() && _logCount < 12) {
        Serial.printf("Frame changed: %d\n", catAnim.currentFrameIndex());
        _logCount++;
    }

    rendererUpdate(catAnim);
}
