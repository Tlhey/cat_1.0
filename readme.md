我的开发板型号：

Waveshare ESP32-S3-Touch-AMOLED-1.8，V1 版本

主控：
ESP32-S3R8
Xtensa 32-bit LX7 双核，240 MHz
内置 8 MB PSRAM
外接 16 MB Flash

屏幕：
1.8 英寸电容触摸 AMOLED
分辨率 368 × 448
屏幕驱动芯片：SH8601
屏幕通信：QSPI
颜色：16.7M 色

触摸：
触摸芯片：FT3168
触摸通信：I2C
I2C 地址：0x38
触摸需要使用 Waveshare 官方 Arduino_DriveBus_Library 读取，不要直接裸读寄存器

常用引脚：
LCD_SDIO0 = 4
LCD_SDIO1 = 5
LCD_SDIO2 = 6
LCD_SDIO3 = 7
LCD_SCLK  = 11
LCD_CS    = 12

I2C_SDA = 15
I2C_SCL = 14
TP_INT  = 21

I2C 扫描结果：
0x18：ES8311 音频 Codec
0x20：XCA9554 / TCA9554 GPIO 扩展器
0x34：AXP2101 电源管理芯片
0x38：FT3168 触摸芯片
0x51：PCF85063 RTC
0x6B：QMI8658 六轴 IMU
0x7E：其他板载 I2C 设备/保留设备

Arduino IDE 板型：
Waveshare ESP32-S3-Touch-AMOLED-1.8

推荐库：
GFX_Library_for_Arduino
Arduino_DriveBus_Library
Adafruit_XCA9554
ArduinoJson
WiFi
HTTPClient

屏幕初始化方式：
Arduino_ESP32QSPI + Arduino_SH8601
不要用 Arduino_CO5300，那是 V2 屏幕驱动

触摸初始化方式：
Arduino_DriveBus_Library 里的 Arduino_FT3x68
不要用普通 FT6x36 裸寄存器读取方式，因为会读到全 0

已经验证成功的功能：
Serial Monitor 正常
SH8601 屏幕显示正常
FT3168 触摸正常
触摸按钮切换页面正常
Wi-Fi 扫描正常
Wi-Fi 连接正常
HTTP GET 正常
JSON API 解析正常