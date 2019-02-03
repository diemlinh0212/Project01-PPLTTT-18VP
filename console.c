#include "console.h"

//==================================================

COORD createCoordObject(int x, int y) {
  COORD xy;
  xy.X = x;
  xy.Y = y;
  return xy;
}

SMALL_RECT createSmallRectObject(int Top, int Left, int Bottom, int Right) {
  SMALL_RECT sr;
  sr.Top = Top;
  sr.Left = Left;
  sr.Bottom = Bottom;
  sr.Right = Right;
  return sr;
}

//==================================================

int COORD2int(COORD xy, COORD xySize) {
  if (xy.X >= xySize.X || xy.Y >= xySize.Y) return -1;
  return ((int)xy.X) + ((int)xy.Y)*((int)xySize.X);  
}

//==================================================

static int isGetConsoleWindow = 0;
static HWND hConsoleWindow;
HWND getConsoleWindow() {
  if (!isGetConsoleWindow) {
    isGetConsoleWindow = 1;
    hConsoleWindow = GetConsoleWindow();
  }
  return hConsoleWindow;
}

static int isGetStdOutputHandle = 0;
static HANDLE hStdOutput;
HANDLE getStdOutputHandle() {
  if (!isGetStdOutputHandle) {
    isGetStdOutputHandle = 1;
    hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  }
  return hStdOutput;
}

//==================================================

CONSOLE_SCREEN_BUFFER_INFO getConsoleScreenBufferInfo() {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(getStdOutputHandle(), &csbi);
  return csbi;
}

static int isGetConsoleMaximumSize = 0;
static COORD xyConsoleMaximumSize;
COORD getConsoleMaximumSize() {
  if (!isGetConsoleMaximumSize) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    isGetConsoleMaximumSize = 1;
    csbi = getConsoleScreenBufferInfo();
    xyConsoleMaximumSize = createCoordObject(csbi.dwMaximumWindowSize.X, csbi.dwMaximumWindowSize.Y-1);
  }
  return xyConsoleMaximumSize;
}

//==================================================

void setFullScreenConsole() {
  ShowWindow(getConsoleWindow(), SW_MAXIMIZE);
}

void setConsoleCursorPosition(int x, int y) {
  SetConsoleCursorPosition(getStdOutputHandle(), createCoordObject(x, y));
}

void initConsole() {
  COORD xySize = getConsoleMaximumSize();
  COORD xyHome = createCoordObject(0, 0);
  DWORD count;
  setFullScreenConsole();
  FillConsoleOutputCharacter(getStdOutputHandle(), ' ', ((int)xySize.X)*((int)xySize.Y), xyHome, &count);
  FillConsoleOutputAttribute(getStdOutputHandle(), getConsoleScreenBufferInfo().wAttributes, ((int)xySize.X)*((int)xySize.Y), xyHome, &count);
  setConsoleCursorPosition(0, 0);
}

void readFullConsoleBuffer(CHAR_INFO buffer[]) {
  COORD xySize = getConsoleMaximumSize();
  COORD xyHome = createCoordObject(0, 0);
  SMALL_RECT smallRect = createSmallRectObject(0, 0, xySize.Y-1, xySize.X-1);
  ReadConsoleOutput(getStdOutputHandle(), buffer, getConsoleMaximumSize(), xyHome, &smallRect);
}

void writeFullConsoleBuffer(CHAR_INFO buffer[]) {
  COORD xySize = getConsoleMaximumSize();
  COORD xyHome = createCoordObject(0, 0);
  SMALL_RECT smallRect = createSmallRectObject(0, 0, xySize.Y-1, xySize.X-1);
  WriteConsoleOutput(getStdOutputHandle(), buffer, getConsoleMaximumSize(), xyHome, &smallRect);
  setConsoleCursorPosition(0, 0);
}

//==================================================

void delayFPS(int FPS) {
  Sleep(1000/FPS);
}
