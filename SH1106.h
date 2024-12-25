#ifndef SH1106_H
#define SH1106_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define SH1106_WIDTH  128  // Display width in pixels
#define SH1106_HEIGHT 128  // Display height in pixels
#define SH1106_ADDR   0x3C  // I2C address of the SH1106 OLED

#define SH1106_I2C_PORT i2c0
#define SH1106_SDA_PIN 16
#define SH1106_SCL_PIN 17
#define SH1106_OLED_ADDR 0x3C  // Address for SH1106

// Initialize I2C
void SH1106_I2CInitOled(void);
void SH1106_Init(void);
void SH1106_Clear(void);
void SH1106_Display(void);
void SH1106_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void SH1106_SendCommand(uint8_t command);
void SH1106_SendData(uint8_t data);

void SSH1106_DrawChar(uint8_t x, uint8_t y, char c);
void SH1106_DrawString(uint8_t x, uint8_t y, const char *str);

#endif // SH1106_H
