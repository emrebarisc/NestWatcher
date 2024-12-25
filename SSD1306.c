#include "SSD1306.h"

#include <string.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "Font.h"

// Send command to OLED
void SSD1306_SendCommand(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd}; // 0x00 for command
    i2c_write_blocking(I2C_PORT, SSD1306_I2C_ADDR, data, 2, false);
}

// Send data to OLED
void SSD1306_SendData(uint8_t* data, size_t length)
{
    uint8_t buffer[length + 1];
    buffer[0] = 0x40; // 0x40 for data
    memcpy(&buffer[1], data, length);
    i2c_write_blocking(I2C_PORT, SSD1306_I2C_ADDR, buffer, length + 1, false);
}

// Initialize I2C
void SSD1306_I2CInitOled()
{
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA_PIN);
    gpio_pull_up(SSD1306_SCL_PIN);
}

// Initialize the OLED display
void SSD1306_Init()
{
    SSD1306_I2CInitOled();
    
    sleep_ms(100); // Wait for display to power on

    // Initialization sequence
    SSD1306_SendCommand(0xAE); // Display OFF
    SSD1306_SendCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_SendCommand(0x80);
    SSD1306_SendCommand(0xA8); // Set multiplex ratio
    SSD1306_SendCommand(0x3F); // 1/64 duty
    SSD1306_SendCommand(0xD3); // Set display offset
    SSD1306_SendCommand(0x00);
    SSD1306_SendCommand(0x40); // Set start line address
    SSD1306_SendCommand(0x8D); // Enable charge pump
    SSD1306_SendCommand(0x14);
    SSD1306_SendCommand(0x20); // Set memory addressing mode
    SSD1306_SendCommand(0x00); // Horizontal addressing mode
    SSD1306_SendCommand(0xA1); // Set segment re-map
    SSD1306_SendCommand(0xC8); // Set COM output scan direction
    SSD1306_SendCommand(0xDA); // Set COM pins hardware configuration
    SSD1306_SendCommand(0x12);
    SSD1306_SendCommand(0x81); // Set contrast control
    SSD1306_SendCommand(0xCF);
    SSD1306_SendCommand(0xD9); // Set pre-charge period
    SSD1306_SendCommand(0xF1);
    SSD1306_SendCommand(0xDB); // Set VCOMH deselect level
    SSD1306_SendCommand(0x40);
    SSD1306_SendCommand(0xA4); // Disable entire display on
    SSD1306_SendCommand(0xA6); // Set normal display
    SSD1306_SendCommand(0xAF); // Display ON
}

// Clear the display buffer
void SSD1306_ClearBuffer()
{
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

// Update the display with the buffer
void SSD1306_UpdateDisplay()
{
    for (uint8_t i = 0; i < 8; i++)
    { // 8 pages for 64 rows
        SSD1306_SendCommand(0xB0 + i); // Set page start address
        SSD1306_SendCommand(0x00);     // Set low column address
        SSD1306_SendCommand(0x10);     // Set high column address
        SSD1306_SendData(&oled_buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
    }
}

// Set a pixel in the buffer
void SSD1306_SetPixel(int x, int y, bool on)
{
    if (x >= 0 && x < SSD1306_WIDTH && y >= 0 && y < SSD1306_HEIGHT)
    {
        if (on)
        {
            oled_buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
        } else {
            oled_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
        }
    }
}

// Example pattern
void SSD1306_DrawExamplePattern()
{
    for (int x = 0; x < SSD1306_WIDTH; x++)
    {
		if(KURUKAFA_WIDTH <= x) continue;
        for (int y = 0; y < SSD1306_HEIGHT; y++)
        {
			if(KURUKAFA_HEIGHT <= y) continue;
            SSD1306_SetPixel(x, y, 0 < kurukafa[y * SSD1306_WIDTH + x] ? 1 : 0);

			// char kurukafaValue = kurukafa[y * SSD1306_WIDTH + x];
			// for (size_t bit = 0; bit < 8; bit++)
			// {
            // 	SSD1306_SetPixel(x, y, (kurukafaValue >> bit) & 0x01);
			// }
        }
    }
}

void SSD1306_DrawChar(int x, int y, char c, bool on)
{
    if (c < 32 || c > 127)
    {
        c = 32; // Default to space for unsupported characters
    }
    const uint8_t* char_bitmap = font_5x7[c - 32];

    for (int col = 0; col < FONT_WIDTH; col++)
    {
        uint8_t column_data = char_bitmap[col];
        for (int row = 0; row < FONT_HEIGHT; row++)
        {
            bool pixel_on = column_data & (1 << row);
            SSD1306_SetPixel(x + col, y + row, pixel_on && on);
        }
    }
}

void SSD1306_DrawString(int x, int y, const char* str, bool on)
{
    int cursor_x = x;
    while (*str)
    {
        int isNewLine = *str == '\n';

        if( isNewLine || 
            SSD1306_WIDTH <= cursor_x ||
            (SSD1306_WIDTH - cursor_x) < FONT_WIDTH)
        {
            cursor_x = 0;
            y += FONT_HEIGHT + 1;
            if(isNewLine)
            {
                str++;
                continue;
            }
        }

        SSD1306_DrawChar(cursor_x, y, *str, on);
        cursor_x += FONT_WIDTH + FONT_SPACING;
        str++;
    }
}