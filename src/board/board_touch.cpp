#include "board_touch.h"
#include <Arduino_DriveBus_Library.h>
#include <memory>

static std::shared_ptr<Arduino_IIC_DriveBus> _iicBus;
static Arduino_IIC* _tp = nullptr;
static bool _prevTouching = false;

void touchBegin() {
    pinMode(TP_INT, INPUT_PULLUP);
    _prevTouching = false;

    // Wire is already initialized by displayBegin(); HWIIC just re-uses it.
    _iicBus = std::make_shared<Arduino_HWIIC>(I2C_SDA, I2C_SCL, &Wire);

    // rst=-1 (no reset pin), iqr=TP_INT (interrupt pin, polled — no callback)
    _tp = new Arduino_FT3x68(
        _iicBus,
        FT3168_DEVICE_ADDRESS,
        DRIVEBUS_DEFAULT_VALUE,   // rst
        TP_INT                    // iqr — sets INPUT_PULLUP, no ISR attached
    );

    if (!_tp->begin()) {
        Serial.println("FT3168 init failed");
        _tp = nullptr;
    }
}

// Returns true exactly once per touch-down; fills (x, y) with screen coords.
bool touchPoll(int16_t& x, int16_t& y) {
    bool touching = (digitalRead(TP_INT) == LOW);

    if (!touching) {
        _prevTouching = false;
        return false;
    }
    if (_prevTouching) return false; // already reported this touch

    _prevTouching = true;
    if (!_tp) return false;

    int n = (int)_tp->IIC_Read_Device_Value(
        Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);
    if (n <= 0) return false;

    x = (int16_t)_tp->IIC_Read_Device_Value(
        Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    y = (int16_t)_tp->IIC_Read_Device_Value(
        Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
    return true;
}
