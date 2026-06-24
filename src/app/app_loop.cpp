#include "app_loop.h"
#include "../board/board_display.h"
#include "../render/renderer.h"
#include "../cat/cat_actor.h"
#include "../cat/cat_random_behavior.h"

static CatActor          catActor;
static CatRandomBehavior catBehavior;
static uint8_t           _logCount = 0;

void appSetup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("CatPet Phase 2 starting...");

    if (!displayBegin()) {
        Serial.println("Display init FAILED");
        while (true) delay(1000);
    }
    Serial.println("Display init OK");

    rendererBegin();
    Serial.println("Renderer init OK");

    catActor.begin();
    catBehavior.begin((uint32_t)esp_random());
    Serial.println("CatActor + RandomBehavior init OK");

    rendererDrawStaticUI();

    // Draw first frame immediately so screen isn't blank.
    catBehavior.update(catActor, millis());
    rendererDrawCatActor(catActor);

    Serial.println("Setup complete");
}

void appLoop() {
    uint32_t now = millis();

    catActor.update(now);
    catBehavior.update(catActor, now);

    if (catActor.needsRedraw()) {
        rendererDrawCatActor(catActor);

        if (_logCount < 24) {
            Serial.printf("[%lu] action=%s frame=%p pos=(%d,%d)\n",
                now,
                catActionName(catActor.currentActionId()),
                catActor.currentFrame(),
                (int)catActor.x(), (int)catActor.y());
            _logCount++;
        }
    }
}
