#include "Wifi.h"

#include "FreeRTOS.h"
#include "task.h"

#include "pico/cyw43_arch.h"

#include "Display/Display.h"

int retryConnectionDelayMs = 1000;
int isWIFIInitialized = 0;

void WIFI_Init(void* parameters)
{    
    // if (cyw43_arch_init())
    // {
    //     Display_GuardedClearFrom(0, 1);
    //     Display_GuardedPrint(0, 8, "Failed to initialise");
    //     printf("Failed to initialise");
    //     return;
    // }

    // cyw43_arch_enable_sta_mode();
    // Display_GuardedPrint(0, 8, "Wifi Initialised");
    // if (cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK))
    // {
    //     Display_GuardedClearFrom(0, 1);
    //     Display_GuardedPrint(0, 8, "Failed to connect");
    //     printf("Failed to connect");
    // }
    // else
    // {
    //     Display_GuardedClear();
    //     Display_GuardedPrintChar(123, 0, 0);
    //     Display_GuardedPrint(0, 8, "Wifi Connected");
    //     printf("Wifi Connected");
    // }
    
    // vTaskDelay(1000);

    // Display_GuardedClear();
    // Display_GuardedPrintChar(123, 0, 0);
    // Display_GuardedPrint(0, 8, "WTF");
    // printf("WTF");
}

void WIFI_Connect(void* parameters)
{
}
