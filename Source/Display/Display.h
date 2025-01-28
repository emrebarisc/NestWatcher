#ifndef __DISPLAY_H__
#define __DISPLAY_H__

void Display_Init();
void Display_GuardedClear();
void Display_GuardedClearFrom(int x, int y);
void Display_GuardedPrint(int x, int y, char* out);
void Display_GuardedPrintChar(int x, int y, char out);
void Display_Update();
void Display_UpdateAsync(void* parameters);

#endif