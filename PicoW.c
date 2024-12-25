#include <stdio.h>
#include <stdlib.h>

#include "pico/cyw43_arch.h"
#include "cyw43.h"
#include "lwipopts.h"

#include "pico/util/datetime.h"

#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "pico/time.h"

#include "SSD1306.h"

void InitWifi()
{
	char log[200];

	if (cyw43_arch_init())
	{
		strcpy(log, "Error initializing Wi-Fi.");
	}
	else
	{
    	cyw43_arch_enable_sta_mode();
		
		if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
		{
			strcpy(log, "Failed to connect to Wi-Fi.");
		} 
		else 
		{
			strcpy(log, "Connected to Wi-Fi successfully!\nYou are now connected to Wi-Fi. Network functionality can be used here.");
		}

		cyw43_arch_deinit();
	}

	SSD1306_ClearBuffer();
	SSD1306_DrawString(0, 0, log, true);
	sleep_ms(2000);
}

int main()
{
    SSD1306_Init();

    SSD1306_ClearBuffer();

    SSD1306_DrawString(0, 0, "Hello, SSD1306!", true);

    // Update the display
    SSD1306_UpdateDisplay();

    // Loop to keep the display updated
    while (true) {
        sleep_ms(1000);  // Sleep for 1 second
        SSD1306_ClearBuffer();
        SSD1306_DrawString(0, 0, "Updated Text!", true);
        SSD1306_UpdateDisplay();
        sleep_ms(1000);  // Sleep for another second
    }

	// SSD1306_Init();

	// // InitWifi();
	
	// while (1)
	// {
	// 	SSD1306_ClearBuffer();
	// 	SSD1306_DrawString(0, 0, "HEEEEEEEEYYYYYY", true, true);
	// 	SSD1306_UpdateDisplay(); 
	// 	sleep_ms(1000);

	// 	SSD1306_ClearBuffer();
	// 	SSD1306_DrawString(0, 0, "HOOOOOOOOOOOOOO", true, true);
	// 	SSD1306_UpdateDisplay();
	// 	sleep_ms(1000);
	// }

	// return 0;
}
