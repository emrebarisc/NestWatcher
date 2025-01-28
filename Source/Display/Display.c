#include "Display.h"

#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "SSD1306.h"
#include "Font.h"

SemaphoreHandle_t printMutex;

void Display_Init()
{
    SSD1306_Init();

    SSD1306_DrawString(43, 28, "Welcome", true);
    SSD1306_UpdateDisplay();
    sleep_ms(500);
    SSD1306_ClearBuffer();
    SSD1306_UpdateDisplay();
    
    printMutex = xSemaphoreCreateMutex();
}

void Display_GuardedClear()
{
    xSemaphoreTake(printMutex, portMAX_DELAY);
    SSD1306_ClearBuffer();
    xSemaphoreGive(printMutex);
}


void Display_GuardedClearFrom(int x, int y)
{
    xSemaphoreTake(printMutex, portMAX_DELAY);
    SSD1306_ClearBufferFrom(x, y);
    xSemaphoreGive(printMutex);
}

void Display_GuardedPrint(int x, int y, char* out)
{
    xSemaphoreTake(printMutex, portMAX_DELAY);
    puts(out);

    SSD1306_DrawString(x, y, out, true);

    xSemaphoreGive(printMutex);
}

void Display_GuardedPrintChar(int x, int y, char out)
{
    xSemaphoreTake(printMutex, portMAX_DELAY);
    
    SSD1306_DrawChar(x, y, out, true);

    xSemaphoreGive(printMutex);
}

void Display_Update()
{
    SSD1306_UpdateDisplay();
}

void Display_UpdateAsync(void* parameters)
{
    while(1)
    {
        SSD1306_UpdateDisplay();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}