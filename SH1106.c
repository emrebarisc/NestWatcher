#include "sh1106.h"
#include "hardware/i2c.h"
#include "Font.h"

static uint8_t buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];  // Buffer for the display (128x128/8 = 1024 bytes)

void SH1106_I2CInitOled(void)
{
    i2c_init(SH1106_I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(SH1106_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SH1106_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SH1106_SDA_PIN);
    gpio_pull_up(SH1106_SCL_PIN);
}

void SH1106_Init(void)
{
    SH1106_I2CInitOled();

    // Initialize I2C communication
    SH1106_SendCommand(0xAE);  // Display off
    SH1106_SendCommand(0xD5);  // Set display clock divide ratio/oscillator frequency
    SH1106_SendCommand(0x80);  // Set the oscillator frequency to 0x80

    SH1106_SendCommand(0xA8);  // Set multiplex ratio (height - 1)
    SH1106_SendCommand(0x7F);  // Set multiplex ratio to 127 (0x7F for a 128-pixel high display)

    SH1106_SendCommand(0xD3);  // Set display offset
    SH1106_SendCommand(0x00);  // No offset

    SH1106_SendCommand(0x40);  // Set display start line

    SH1106_SendCommand(0x8D);  // Enable charge pump regulator
    SH1106_SendCommand(0x14);  // Enable charge pump

    SH1106_SendCommand(0x20);  // Set memory addressing mode
    SH1106_SendCommand(0x00);  // Horizontal addressing mode

    SH1106_SendCommand(0xA1);  // Set segment remap (column address 127 is mapped to SEG0)

    SH1106_SendCommand(0xC8);  // Set COM scan direction (remap scan direction)

    SH1106_SendCommand(0xDA);  // Set COM pins hardware configuration
    SH1106_SendCommand(0x12);  // Sequential COM pins

    SH1106_SendCommand(0x81);  // Set contrast control
    SH1106_SendCommand(0x7F);  // Set contrast to 0x7F (middle)

    SH1106_SendCommand(0xA4);  // Entire display on (default is on)

    SH1106_SendCommand(0xA6);  // Normal display (not inverted)

    SH1106_SendCommand(0xD9);  // Set pre-charge period
    SH1106_SendCommand(0xF1);  // Pre-charge period set to 0xF1

    SH1106_SendCommand(0xDB);  // Set VCOMH deselect level
    SH1106_SendCommand(0x40);  // VCOMH deselect level at 0.77*VCC

    SH1106_SendCommand(0x8D);  // Enable charge pump regulator
    SH1106_SendCommand(0x14);  // Charge pump enabled

    SH1106_SendCommand(0xAF);  // Display ON
}

void SH1106_Clear(void)
{
    // Clear the buffer
    for (int i = 0; i < sizeof(buffer); i++)
    {
        buffer[i] = 0x00;
    }
}

void SH1106_Display(void)
{
    // Send the buffer data to the SH1106 display
    for (uint8_t page = 0; page < 8; page++)
    {
        SH1106_SendCommand(0xB0 + page);  // Set page address
        SH1106_SendCommand(0x00);         // Set column start address low
        SH1106_SendCommand(0x10);         // Set column start address high

        for (uint8_t col = 0; col < SH1106_WIDTH; col++)
        {
            SH1106_SendData(buffer[page * SH1106_WIDTH + col]);
        }
    }
}

void SH1106_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) return;

    uint16_t index = x + (y / 8) * SH1106_WIDTH;
    uint8_t bit = 1 << (y % 8);

    if (color)
    {
        buffer[index] |= bit;  // Set the pixel
    } else {
        buffer[index] &= ~bit;  // Clear the pixel
    }
}

void SH1106_SendCommand(uint8_t command)
{
    uint8_t data[2] = {0x00, command};
    i2c_write_blocking(SH1106_I2C_PORT, SH1106_ADDR, data, 2, false);  // Write the command
}

void SH1106_SendData(uint8_t data)
{
    uint8_t buffer[2] = {0x40, data};
    i2c_write_blocking(SH1106_I2C_PORT, SH1106_ADDR, buffer, 2, false);  // Write the data
}


// Function to draw a character on the OLED display at position (x, y)
void SSH1106_DrawChar(uint8_t x, uint8_t y, char c)
{
    if (c < 32 || c > 126) return; // Handle invalid characters

    // Index into the font array for the character
    uint8_t char_index = c - 32;

    // Loop through each column of the character (FONT_WIDTH columns)
    for (uint8_t col = 0; col < FONT_WIDTH; col++)
    {
        uint8_t col_data = font_5x7[char_index][col];

        // Loop through each row of the character (FONT_HEIGHT rows)
        for (uint8_t row = 0; row < FONT_HEIGHT; row++)
        {
            if (col_data & (1 << row))
            {
                // If bit is set, turn on the pixel at (x + col, y + row)
                SH1106_DrawPixel(x + col, y + row, 1);
            } else {
                // Otherwise, turn off the pixel
                SH1106_DrawPixel(x + col, y + row, 0);
            }
        }
    }
}

// Function to write a string to the OLED display
void SH1106_DrawString(uint8_t x, uint8_t y, const char *str)
{
    while (*str)
    {
        SSH1106_DrawChar(x, y, *str);  // Draw the current character
        x += FONT_WIDTH + FONT_SPACING;  // Move x to the right for the next character
        if (x + FONT_WIDTH > SH1106_WIDTH)
        {  // If the text goes beyond the display width
            x = 0;   // Start a new line (reset x to 0)
            y += FONT_HEIGHT;  // Move down to the next line
            if (y + FONT_HEIGHT > SH1106_HEIGHT)
            { // If the text goes off the screen, clear and reset
                y = 0;
                SH1106_Clear();
            }
        }
        str++;  // Move to the next character
    }
}
