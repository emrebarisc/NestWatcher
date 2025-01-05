#ifndef __SSD1306_H__
#define __SSD1306_H__

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "Font.h"
#include "kurukafaArray.h"

// Define I2C pins and address
#define I2C_PORT i2c0
#define SSD1306_SDA_PIN 16
#define SSD1306_SCL_PIN 17
#define SSD1306_I2C_ADDR 0x3C

// Screen dimensions
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

static uint8_t oled_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

void SSD1306_SendCommand(uint8_t cmd);
void SSD1306_SendData(uint8_t* data, size_t length);
void SSD1306_I2CInitOled();
void SSD1306_Init();
void SSD1306_ClearBuffer();
void SSD1306_ClearBufferFrom(int x, int y);
void SSD1306_UpdateDisplay();
void SSD1306_SetPixel(int x, int y, bool on);
void SSD1306_DrawExamplePattern();
void SSD1306_DrawChar(int x, int y, char c, bool on);
void SSD1306_DrawString(int x, int y, const char* str, bool on);

#endif