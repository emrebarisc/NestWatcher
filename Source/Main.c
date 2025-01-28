#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/altcp_tls.h"

#include "lwip/netif.h"

#include "FreeRTOS.h"
#include "task.h"
#include "Network/HTTPClientUtil.h"

#include "Display/Display.h"

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TASK_PRIORITY ( tskIDLE_PRIORITY + 2UL )
#define TASK_STACK_SIZE 1024

void MainTask(__unused void *params)
{
    if (cyw43_arch_init())
    {
        Display_GuardedClear();
        Display_GuardedPrint(0, 0, "Failed to initialise\n");
        Display_Update();
        return;
    }

    cyw43_arch_enable_sta_mode();
    
    char log[128];

    Display_GuardedClear();
    Display_GuardedPrint(0, 0, "Connecting...\n");
    Display_Update();
    int result = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000);
    if (result)
    {
        sprintf(log, "Failed to connect. Error code: %d\n", result);
        Display_GuardedClear();
        Display_GuardedPrint(0, 0, log);
        Display_Update();
    }
    else
    {
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        sprintf(log, "Connected.\nIP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        
        Display_GuardedClear();
        Display_GuardedPrint(0, 0, log);
        Display_Update();
    }
    
    while(1);
}

void vLaunch( void)
{
    TaskHandle_t task;
    xTaskCreate(MainTask, "TestMainThread", TASK_STACK_SIZE, NULL, TASK_PRIORITY, &task);

    // xTaskCreate(Display_UpdateAsync, "Display_UpdateAsync", TASK_STACK_SIZE, NULL, 0, NULL);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main( void )
{
    stdio_init_all();
    
    Display_Init();

    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    char log[1024];
    printf("Starting %s on both cores.\n", rtos_name);
    printf(log);
    // Display_GuardedClear();
    // Display_GuardedPrint(0, 0, log);
    vLaunch();
#elif ( RUN_FREERTOS_ON_CORE == 1 )
    char log[1024];
    printf("Starting %s on core 1:\n", rtos_name);
    printf(log);
    // Display_GuardedClear();
    // Display_GuardedPrint(0, 0, log);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    char log[1024];
    printf("Starting %s on core 0:\n", rtos_name);
    printf(log);
    // Display_GuardedClear();
    // Display_GuardedPrint(0, 0, log);
    vLaunch();
#endif
    
    cyw43_arch_deinit();
    return 0;
}
