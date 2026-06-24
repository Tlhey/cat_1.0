#include "board_touch.h"
#include <Arduino_DriveBus_Library.h>
#include <memory>

static std::shared_ptr<Arduino_IIC_DriveBus> _iicBus;
static Arduino_IIC* _tp = nullptr;

static bool _prevTouching = false;

void touchBegin() {
    pinMode(TP_INT, INPUT_PULLUP);
    _prevTouching = false;

    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(400000);

    _iicBus = std::make_shared<Arduino_HWIIC>(I2C_SDA, I2C_SCL, &Wire);

    // 重点：
    // 不把 TP_INT 交给库处理。
    // 这里使用纯 I2C 轮询，避免错过 TP_INT 的短脉冲。
    _tp = new Arduino_FT3x68(
        _iicBus,
        FT3168_DEVICE_ADDRESS,
        DRIVEBUS_DEFAULT_VALUE,   // rst
        DRIVEBUS_DEFAULT_VALUE    // irq disabled
    );

    if (!_tp->begin()) {
        Serial.println("FT3168 init failed");
        _tp = nullptr;
        return;
    }

    Serial.println("FT3168 touch OK");
}

bool touchPoll(int16_t& x, int16_t& y) {
    if (!_tp) return false;

    int n = (int)_tp->IIC_Read_Device_Value(
        Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER
    );

    bool touching = (n > 0 && n <= 5);

    if (!touching) {
        _prevTouching = false;
        return false;
    }

    int16_t rawX = (int16_t)_tp->IIC_Read_Device_Value(
        Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X
    );

    int16_t rawY = (int16_t)_tp->IIC_Read_Device_Value(
        Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y
    );

    // 防御性裁剪，避免偶尔读到异常坐标
    if (rawX < 0 || rawX >= SCREEN_W || rawY < 0 || rawY >= SCREEN_H) {
        return false;
    }

    x = rawX;
    y = rawY;

    // 只在“刚按下”的那一下返回 true
    if (_prevTouching) return false;

    _prevTouching = true;

    Serial.printf("touch: x=%d y=%d n=%d\n", x, y, n);

    return true;
}