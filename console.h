#ifndef CONSOLE_H
#define CONSOLE_H

#include "load_lib.h"

// Create objects
COORD createCoordObject(int x, int y);
SMALL_RECT createSmallRectObject(int top, int left, int bottom, int right);

// Converting functions
int coord2int(int x, int y, COORD xySize);
COORD int2coord(int i, COORD xySize);

// Get handles
HWND getConsoleWindow();
HANDLE getStdOutputHandle();

// Get information of console
CONSOLE_SCREEN_BUFFER_INFO getConsoleScreenBufferInfo();
COORD getConsoleMaximumSize();

// Set console
void setFullScreenConsole();
void setConsoleCursorPosition(int x, int y);
void setVisibleCursor(int yesno);
void initConsole();
void readFullConsoleBuffer(CHAR_INFO buffer[]);
void writeFullConsoleBuffer(CHAR_INFO buffer[]);

// Utilities
void delayFPS(int FPS);
void clearBuffer(CHAR_INFO buffer[]);
void drawTextToBuffer(CHAR_INFO buffer[], int x, int y, const char * format, ...);
void fastRemoveSmallRectBuffer(CHAR_INFO buffer[], int top, int left, int bottom, int right);

#endif
