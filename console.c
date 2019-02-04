#include "console.h"

//==================================================

COORD createCoordObject(int x, int y) {
  COORD xy;
  xy.X = x;
  xy.Y = y;
  return xy;
}

SMALL_RECT createSmallRectObject(int top, int left, int bottom, int right) {
  SMALL_RECT sr;
  sr.Top = top;
  sr.Left = left;
  sr.Bottom = bottom;
  sr.Right = right;
  return sr;
}

//==================================================

int COORD2int(int x, int y, COORD xySize) {
  if (x < 0 || y < 0 || x >= xySize.X || y >= xySize.Y) return -1;
  return x + y*((int)xySize.X);  
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

void setVisibleCursor(int yesno) {
  CONSOLE_CURSOR_INFO cci;
  GetConsoleCursorInfo(getStdOutputHandle(), &cci);
  cci.bVisible = yesno;
  SetConsoleCursorInfo(getStdOutputHandle(), &cci);
}

void initConsole() {
  COORD xySize = getConsoleMaximumSize();
  COORD xyHome = createCoordObject(0, 0);
  DWORD count;
  setFullScreenConsole();
  setVisibleCursor(0);
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

void clearBuffer(CHAR_INFO buffer[]) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  COORD xySize;
  int i;
  csbi = getConsoleScreenBufferInfo();
  xySize = getConsoleMaximumSize();
  for (i = 0; i < ((int)xySize.X)*((int)xySize.Y); i++) {
    buffer[i].Char.AsciiChar = ' ';
    buffer[i].Attributes = csbi.wAttributes;
  }
}

void drawTextToBuffer(CHAR_INFO buffer[], int x, int y, const char * format, ...) {
  char strBuffer[256];
  va_list args;
  int len_strBuffer;
  COORD xySize;
  int i;
  va_start(args, format);
  vsnprintf(strBuffer, 256, format, args);
  va_end(args);
  len_strBuffer = strlen(strBuffer);
  xySize = getConsoleMaximumSize();
  for (i = x; (i < xySize.X) && (i-x < len_strBuffer); i++) {
    buffer[COORD2int(i, y, xySize)].Char.AsciiChar = strBuffer[i-x];
  }
}

void fastRemoveSmallRectBuffer(CHAR_INFO buffer[], int top, int left, int bottom, int right) {
  COORD xySize;
  int i, j, pos;
  xySize = getConsoleMaximumSize();
  for (i = left; i <= right; i++) {
    for (j = top; j <= bottom; j++) {
      pos = COORD2int(i, j, xySize);
      if (pos > 0)
        buffer[pos].Char.AsciiChar = ' ';
    }
  }
}
