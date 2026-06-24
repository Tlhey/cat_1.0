#include "board_display.h"
#include <Wire.h>

static Arduino_DataBus* _bus = nullptr;
static Arduino_GFX*     _gfx = nullptr;

static bool _tryXCA9554(uint8_t addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() != 0) return false;
    Wire.beginTransmission(addr);
    Wire.write(0x03); Wire.write(0x00);   // config: all output
    Wire.endTransmission();
    Wire.beginTransmission(addr);
    Wire.write(0x01); Wire.write(0xFF);   // output: all HIGH (LCD_EN on)
    Wire.endTransmission();
    return true;
}

static void _powerOnDisplay() {
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(10);
    // Waveshare V1: XCA9554 usually at 0x3F; some revisions use 0x20.
    if (!_tryXCA9554(0x3F)) _tryXCA9554(0x20);
    delay(120);
}

bool displayBegin() {
    _powerOnDisplay();

    _bus = new Arduino_ESP32QSPI(
        LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3
    );
    _gfx = new Arduino_SH8601(_bus, GFX_NOT_DEFINED, 0);

    if (!_gfx->begin()) return false;
    _gfx->fillScreen(COLOR_BG);
    return true;
}

Arduino_GFX* getDisplay() { return _gfx; }

void displayClear(uint16_t color) {
    if (_gfx) _gfx->fillScreen(color);
}
