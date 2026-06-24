#include "app_loop.h"
#include "../board/board_display.h"
#include "../board/board_touch.h"
#include "../render/renderer.h"
#include "../cat/cat_pet.h"

static CatPet catPet;

void appSetup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("CatPet starting...");

    if (!displayBegin()) {
        Serial.println("Display FAILED");
        while (true) delay(1000);
    }
    Serial.println("Display OK");

    touchBegin();

    rendererBegin();
    catPet.begin((uint32_t)esp_random());
    Serial.println("CatPet ready");

    rendererDrawStaticUI();
    rendererDrawCatPet(catPet);
}

void appLoop() {
    uint32_t now = millis();

    int16_t tx, ty;
    if (touchPoll(tx, ty)) {
        catPet.onTouch(tx, ty, now);
    }

    catPet.update(now);
    rendererDrawCatPet(catPet);
}
