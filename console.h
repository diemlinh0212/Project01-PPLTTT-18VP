#ifndef CONSOLE_H
#define CONSOLE_H

#include "load_lib.h"

// Create objects
COORD createCoordObject(int x, int y);
SMALL_RECT createSmallRectObject(int Top, int Left, int Bottom, int Right);

// Converting functions
int COORD2int(COORD xy, COORD xySize);

// Get handles
HWND getConsoleWindow();
HANDLE getStdOutputHandle();

// Get information of console
CONSOLE_SCREEN_BUFFER_INFO getConsoleScreenBufferInfo();
COORD getConsoleMaximumSize();

// Set console
void setFullScreenConsole();
void setConsoleCursorPosition(int x, int y);
void initConsole();
void readFullConsoleBuffer(CHAR_INFO buffer[]);
void writeFullConsoleBuffer(CHAR_INFO buffer[]);

// Utilities
void delayFPS(int FPS);

#endif
