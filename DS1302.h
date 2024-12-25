#ifndef __DS1302_H__
#define __DS1302_H__

#include "pico/stdlib.h"

#define PIN_DS1302_RST 13
#define PIN_DS1302_DAT 14
#define PIN_DS1302_CLK 15

void DS1302_Init()
{
	gpio_init(PIN_DS1302_RST);
	gpio_set_dir(PIN_DS1302_RST, 1);
	gpio_set_function(PIN_DS1302_RST, GPIO_FUNC_SIO);
	gpio_put(PIN_DS1302_RST, 0);

	gpio_init(PIN_DS1302_CLK);
	gpio_set_dir(PIN_DS1302_CLK, 1);
	gpio_set_function(PIN_DS1302_CLK, GPIO_FUNC_SIO);
	gpio_put(PIN_DS1302_CLK, 0);

	int direction = 0;
	gpio_init(PIN_DS1302_DAT);
	gpio_set_function(PIN_DS1302_DAT, GPIO_FUNC_SIO);
	gpio_put(PIN_DS1302_DAT, direction);
}

void GetSecond(uint8_t** second)
{
	gpio_set_dir(PIN_DS1302_DAT, 1);
    gpio_put(PIN_DS1302_RST, 1);

	char data = 0x80;
    for(int i = 0; i < 8; i++)
    {
        gpio_put(PIN_DS1302_CLK, 0);
        gpio_put(PIN_DS1302_DAT, data & 0x01);
		data >>= 1;
        gpio_put(PIN_DS1302_CLK, 1);
    }
	
	uint8_t readValue = 0;
	gpio_set_dir(PIN_DS1302_DAT, 0);
    for(int i = 1; i < 8; i++)
    {
        gpio_put(PIN_DS1302_CLK, 0);
        sleep_us(1);
        uint8_t bit = gpio_get(PIN_DS1302_DAT);
		readValue |= bit << i;
        gpio_put(PIN_DS1302_CLK, 1);
    }

	readValue &= 0b01111111;

	**second = (readValue / 16 * 10) + (readValue % 16);

    gpio_put(PIN_DS1302_RST, 0);
}

#endif