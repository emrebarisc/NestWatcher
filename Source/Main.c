#include "FreeRTOS.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "Display/SSD1306.h"

const int taskDelay = 500;
const int taskSize = 128;

const int LED_PIN = 0;

SemaphoreHandle_t mutex;

int currentLine = 0;

void vGuardedPrint(char* out)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    puts(out);

    currentLine = (currentLine + 1) % 8;
    SSD1306_DrawString(0, currentLine * (FONT_HEIGHT + 1), out, true);

    xSemaphoreGive(mutex);
}

void vTaskSMP_PrintCore(void* parameters)
{
    char* taskName = pcTaskGetName(NULL);
    char out[12];

    while(1)
    {
        sprintf(out, "%s %d", taskName, get_core_num());
        vGuardedPrint(out);
        vTaskDelay(pdMS_TO_TICKS(taskDelay));
    }
}

void vTaskSMP_BlinkLED(void* parameters)
{
    while(1)
    {
        gpio_put(LED_PIN, !gpio_get_out_level(LED_PIN));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vTaskSMP_UpdateDisplay(void* parameters)
{
    while(1)
    {
        SSD1306_UpdateDisplay();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main()
{
    stdio_init_all();

    SSD1306_Init();

    SSD1306_DrawString(43, 28, "Welcome", true);
    SSD1306_UpdateDisplay();
    sleep_ms(500);
    SSD1306_ClearBuffer();
    SSD1306_UpdateDisplay();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    mutex = xSemaphoreCreateMutex();

    TaskHandle_t handleA;
    TaskHandle_t handleB;

    xTaskCreate(vTaskSMP_PrintCore, "A", taskSize, NULL, 1, &handleA);
    xTaskCreate(vTaskSMP_PrintCore, "B", taskSize, NULL, 1, &handleB);
    xTaskCreate(vTaskSMP_PrintCore, "C", taskSize, NULL, 1, NULL);
    xTaskCreate(vTaskSMP_PrintCore, "D", taskSize, NULL, 1, NULL);
    xTaskCreate(vTaskSMP_BlinkLED, "BlinkLED", taskSize, NULL, 1, NULL);
    xTaskCreate(vTaskSMP_UpdateDisplay, "UpdateDisplay", taskSize, NULL, 1, NULL);

    vTaskCoreAffinitySet(handleA, (1 << 0));
    vTaskCoreAffinitySet(handleB, (1 << 1));

    vTaskStartScheduler();
}
