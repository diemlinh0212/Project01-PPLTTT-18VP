#include "demo.h"

#define FRAME_PER_SECOND 30
#define VELOCITY 10.0

static int isEsc = 0;
static int isPlay = 0;
static int isGameOver = 0;
static int stateKeyCode = 0;
static int messageTime = 0;
static char messageStr[256];
static int mapGame[256][256];
static unsigned int countTime = 0;
static COORD headSnake;
static int directionSnake;

void processKeyEventsInDemo(KEY_EVENT_RECORD event) {
  if (event.bKeyDown)
    return;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      isEsc = 1;
      break;
    case VK_RETURN:
      isPlay = 1-isPlay;
      if (isPlay && !isGameOver) {
        messageTime = FRAME_PER_SECOND*2;
        strncpy(messageStr, "Game resumed ...", 256);
      }
      break;
    case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
      stateKeyCode = event.wVirtualKeyCode;
      break;
    default:
      messageTime = FRAME_PER_SECOND*2;
      strncpy(messageStr, "Error: This virtual key code is undefined", 256);
  }
}

void processConsoleInDemo() {
  CHAR_INFO buffer[65536];
  COORD xySize;
  WORD attribute;
  int i, j;
  clearBuffer(buffer);
  xySize = getConsoleMaximumSize();
  attribute = getConsoleScreenBufferInfo().wAttributes;
  if (isPlay && !isGameOver)
    nextFrameGameInDemo();
  for (i = 1; i < xySize.X-1; i++) {
    buffer[COORD2int(i, 0, xySize)].Char.AsciiChar = '\xcd';
    buffer[COORD2int(i, xySize.Y-2, xySize)].Char.AsciiChar = '\xcd';
  }
  for (i = 1; i < xySize.Y-2; i++) {
    buffer[COORD2int(0, i, xySize)].Char.AsciiChar = '\xba';
    buffer[COORD2int(xySize.X-1, i, xySize)].Char.AsciiChar = '\xba';
  }
  buffer[COORD2int(0, 0, xySize)].Char.AsciiChar = '\xc9';
  buffer[COORD2int(xySize.X-1, 0, xySize)].Char.AsciiChar = '\xbb';
  buffer[COORD2int(0, xySize.Y-2, xySize)].Char.AsciiChar = '\xc8';
  buffer[COORD2int(xySize.X-1, xySize.Y-2, xySize)].Char.AsciiChar = '\xbc';
  for (i = 0; i < xySize.X-2; i++) {
    for (j = 0; j < xySize.Y-3; j++) {
      if (mapGame[i][j] > 0) {
        buffer[COORD2int(i+1, j+1, xySize)].Char.AsciiChar = '\xdb';
        buffer[COORD2int(i+1, j+1, xySize)].Attributes = ((attribute & (~FOREGROUND_RED)) | FOREGROUND_GREEN) & (~FOREGROUND_BLUE);
      } else if (mapGame[i][j] == -1) {
        buffer[COORD2int(i+1, j+1, xySize)].Char.AsciiChar = '\xc5';
        buffer[COORD2int(i+1, j+1, xySize)].Attributes = ((attribute | FOREGROUND_RED) | FOREGROUND_GREEN) & (~FOREGROUND_BLUE);
      }
    }
  }
  if (messageTime) {
    messageTime--;
    drawTextToBuffer(buffer, 0, xySize.Y-1, "%s", messageStr);
  } else if (isGameOver) {
    drawTextToBuffer(buffer, 0, xySize.Y-1, "%s", "Game Over ...");
  } else if (!isPlay) {
    drawTextToBuffer(buffer, 0, xySize.Y-1, "%s", "Game Paused ...");
  }
  writeFullConsoleBuffer(buffer);
}

void initGameInDemo() {
  int i, j;
  COORD xySize;
  xySize = getConsoleMaximumSize();
  srand(time(NULL));
  initConsole();
  for (i = 0; i < xySize.X-2; i++) {
    for (j = 0; j < xySize.Y-3; j++) {
      mapGame[i][j] = 0;
    }
  }
  mapGame[0][0] = 1;
  mapGame[1][0] = 2;
  headSnake = createCoordObject(1, 0);
  directionSnake = VK_RIGHT;
  nextFood();
}

void nextFrameGameInDemo() {
  countTime++;
  if (countTime % (int)(FRAME_PER_SECOND/VELOCITY) == 0) {
    int xNext, yNext;
    COORD xySize;
    xNext = headSnake.X;
    yNext = headSnake.Y;
    xySize = getConsoleMaximumSize();
    switch (stateKeyCode) {
      case VK_LEFT:
        if (directionSnake != VK_RIGHT)
          directionSnake = stateKeyCode;
        break;
      case VK_UP:
        if (directionSnake != VK_DOWN)
          directionSnake = stateKeyCode;
        break;
      case VK_RIGHT:
        if (directionSnake != VK_LEFT)
          directionSnake = stateKeyCode;
        break;
      case VK_DOWN:
        if (directionSnake != VK_UP)
          directionSnake = stateKeyCode;
        break;
    }
    switch (directionSnake) {
      case VK_LEFT:
        xNext--;
        break;
      case VK_UP:
        yNext--;
        break;
      case VK_RIGHT:
        xNext++;
        break;
      case VK_DOWN:
        yNext++;
        break;
    }
    if (xNext < 0 || yNext < 0 || xNext >= xySize.X-2 || yNext >= xySize.Y-3) {
      isGameOver = 1;
    } else if (mapGame[xNext][yNext] == -1) {
      mapGame[xNext][yNext] = mapGame[headSnake.X][headSnake.Y] + 1;
      headSnake = createCoordObject(xNext, yNext);
      nextFood();
    } else {
      int i, j;
      for (i = 0; i < xySize.X-2; i++) {
        for (j = 0; j < xySize.Y-3; j++) {
          if (mapGame[i][j] > 0)
            mapGame[i][j]--;
        }
      }
      if (mapGame[xNext][yNext] > 0) {
        isGameOver = 1;
      } else {
        mapGame[xNext][yNext] = mapGame[headSnake.X][headSnake.Y] + 1;
        headSnake = createCoordObject(xNext, yNext);
      }
    }
  }
}

void nextFood() {
  COORD xySize;
  int i, j, nEmptyCell;
  xySize = getConsoleMaximumSize();
  nEmptyCell = 0;
  for (i = 0; i < xySize.X-2; i++) {
    for (j = 0; j < xySize.Y-3; j++) {
      if (mapGame[i][j] == 0)
        nEmptyCell++; 
    }
  }
  if (nEmptyCell == 0) {
    isGameOver = 1;
  } else {
    int randNextFood;
    randNextFood = rand() % nEmptyCell + 1;
    for (i = 0; i < xySize.X-2; i++) {
      for (j = 0; j < xySize.Y-3; j++) {
        if (mapGame[i][j] == 0) {
          randNextFood--;
          if (randNextFood == 0)
            mapGame[i][j] = -1;
        }
      }
    }
  }
}

void runDemo() {
  printf("Demo:\n");
  printf("- Play/Pause: <Enter>\n");
  printf("- Exit      : <Esc>\n");
  printf("- Left      : <Left Arrow>\n");
  printf("- Up        : <Up Arrow>\n");
  printf("- Right     : <Right Arrow>\n");
  printf("- Down      : <Down Arrow>\n");
  system("pause");
  initGameInDemo();
  addKeyEventFunction(&processKeyEventsInDemo);
  while (1) {
    delayFPS(FRAME_PER_SECOND);
    processEventsNonBlocking();
    if (isEsc)
      break;
    processConsoleInDemo();
  }
  removeKeyEventFunction();
}
