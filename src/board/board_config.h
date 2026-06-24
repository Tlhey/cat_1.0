#pragma once

// ── QSPI Display ──────────────────────────────────────────────
#define LCD_SDIO0  4
#define LCD_SDIO1  5
#define LCD_SDIO2  6
#define LCD_SDIO3  7
#define LCD_SCLK  11
#define LCD_CS    12

// ── I2C (Touch + GPIO Expander) ───────────────────────────────
#define I2C_SDA   15
#define I2C_SCL   14
#define TP_INT    21

// ── Screen ────────────────────────────────────────────────────
#define SCREEN_W  368
#define SCREEN_H  448

// ── Cat render ────────────────────────────────────────────────
#define CAT_SCALE    6          // nearest-neighbour upscale factor
// CAT_DRAW_W/H = 32 * 6 = 192 px (computed where CAT_FRAME_W/H are visible)

// ── Colors (RGB565) ───────────────────────────────────────────
#define COLOR_BLACK  0x0000
#define COLOR_WHITE  0xFFFF
#define COLOR_BG     0x18E3   // dark blue-gray background
#define COLOR_TITLE  0xFFFF
#define COLOR_INFO   0xAD55   // muted green
