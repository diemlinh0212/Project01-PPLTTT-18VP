#include "game.h"

static int   currentFrame = 0;
static Scene currentScene = INTRO;
static int   currentSelection = 0;
static int   highScore = 0;
static int   isExitGameVariable = 0;
static int   isGameOverVariable = 0;
static int   isHavingUncompletingGameVariable = 0;
static int   isInitRandom = 0;
static int   isPauseVariable = 1;
static Level levelGame = EASY;
static Map   mapGame;
static int   numberPlayers = 1;
static Snake players[2];
static int   temporaryNumberPlayers = 1;

//==================================================

WORD changeForegroundColor(WORD attributes, Color code, int isIntensity) {
  attributes &= (~FOREGROUND_BLUE) & (~FOREGROUND_GREEN) & (~FOREGROUND_RED) & (~FOREGROUND_INTENSITY);
  switch (code) {
    case RED:
      attributes = attributes | FOREGROUND_RED;
      break;
    case GREEN:
      attributes = attributes | FOREGROUND_GREEN;
      break;
    case BLUE:
      attributes = attributes | FOREGROUND_BLUE;
      break;
    case YELLOW:
      attributes = attributes | FOREGROUND_RED | FOREGROUND_GREEN;
      break;
    case CYAN:
      attributes = attributes | FOREGROUND_GREEN | FOREGROUND_BLUE;
      break;
    case MAGENTA:
      attributes = attributes | FOREGROUND_BLUE | FOREGROUND_RED;
      break;
    case WHITE:
      attributes = attributes | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
      break;
    case BLACK:
      attributes = attributes;
      break;
  }
  if (isIntensity)
    attributes = attributes | FOREGROUND_INTENSITY;
  return attributes;
}

int nextHeadSnake(Snake player, COORD *nextHead) {
  if (nextHead == NULL)
    return -1;
  *nextHead = player.head;
  switch (player.direction) {
    case UP:
      if (nextHead->Y == 0)
        return -1;
      nextHead->Y -= 1;
      break;
    case DOWN:
      if (nextHead->Y == mapGame.size.Y-1)
        return -1;
      nextHead->Y += 1;
      break;
    case LEFT:
      if (nextHead->X == 0)
        return -1;
      nextHead->X -= 1;
      break;
    case RIGHT:
      if (nextHead->X == mapGame.size.X-1)
        return -1;
      nextHead->X += 1;
      break;
  }
}

int nextRandom() {
  int i, nEmpty, nxtRand;
  if (!isInitRandom) {
    isInitRandom = 1;
    srand(time(NULL));
  }
  nEmpty = 0;
  for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
    if (mapGame.buffer[i] == 0)
      nEmpty++;
  }
  if (nEmpty == 0)
    return -1;
  nxtRand = rand() % nEmpty + 1;
  for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
    if (mapGame.buffer[i] == 0) {
      nxtRand--;
      if (nxtRand == 0)
        return i;
    }
  }
}

//==================================================

Scene getCurrentScene() {
  return currentScene;
}

int getFoodValue() {
  switch (levelGame) {
    case EASY:
      return 1;
    case MEDIUM:
      return 5;
    case HARD:
      return 10;
  }
}

ResultMatch getResultMatch() {
  switch (isGameOverVariable) {
    case 1:
      return PLAYER_1;
    case 2:
      return PLAYER_2;
    case 3:
      if (players[0].score > players[1].score)
        return PLAYER_1;
      else if (players[0].score < players[1].score)
        return PLAYER_2;
      else
        return TIE;
    default:
      return UNKNOWN;
  }
}

int getSpeedSnake() {
  switch (levelGame) {
    case EASY:
      return 10;
    case MEDIUM:
      return 15;
    case HARD:
      return 30;
  }
}

int isExitGame() {
  return isExitGameVariable;
}

int isGameOver() {
  return (isGameOverVariable == 0) ? 0 : 1;
}

int isHavingUncompletingGame() {
  return isHavingUncompletingGameVariable;
}

int isPause() {
  return isPauseVariable;
}

//==================================================

void setNewMap() {
  int i;
  COORD sizeScreen = getConsoleMaximumSize();
  mapGame.size = createCoordObject(sizeScreen.X-2, sizeScreen.Y-3);
  for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++)
    mapGame.buffer[i] = 0;
}

void setFood() {
  int i, nEmpty, nFood;
  nEmpty = nFood = 0;
  for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
    if (mapGame.buffer[i] == 0)
      nEmpty++;
    if (mapGame.buffer[i] < 0)
      nFood++;
  }
  while (nFood == 0 || nFood*99 < nEmpty) {
    i = nextRandom();
    if (i == -1)
      break;
    mapGame.buffer[i] = -getFoodValue();
    nFood++;
    nEmpty--;
  }
}

void setNewGame(int nPlayers, Level level) {
  isHavingUncompletingGameVariable = 1;
  isPauseVariable = 1;
  isGameOverVariable = 0;
  currentFrame = 0;
  setNewMap();
  numberPlayers = nPlayers;
  if (numberPlayers == 2) {
    players[1].head = createCoordObject(mapGame.size.X-2, mapGame.size.Y-1);
    players[1].direction = LEFT;
    players[1].length = 2;
    players[1].score = 0;
    mapGame.buffer[coord2int(mapGame.size.X-1, mapGame.size.Y-1, mapGame.size)] = 1;
    mapGame.buffer[coord2int(mapGame.size.X-2, mapGame.size.Y-1, mapGame.size)] = 3;
  }
  players[0].head = createCoordObject(1, 0);
  players[0].direction = RIGHT;
  players[0].length = 2;
  players[0].score = 0;
  mapGame.buffer[coord2int(0, 0, mapGame.size)] = 2;
  mapGame.buffer[coord2int(1, 0, mapGame.size)] = 4;
  levelGame = level;
  setFood();
}

void setGameOver(int winnerPlayer) {
  isGameOverVariable |= winnerPlayer;
  isHavingUncompletingGameVariable = 0;
  if (numberPlayers == 2)
    saveHighScore(players[1].score);
  saveHighScore(players[0].score);
}

void setNextFrameInGame() {
  int i;
  COORD nextHead[2];
  currentFrame++;
  if (currentFrame % (60/getSpeedSnake()) != 0)
    return;
  if (numberPlayers == 1) {
      if (nextHeadSnake(players[0], &nextHead[0]) == -1)
        setGameOver(1);
      else if (mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] < 0) {
        players[0].head = nextHead[0];
        players[0].length++;
        players[0].score += -mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)];
        mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] = players[0].length*2;
        setFood();
      } else {
        for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
          if (mapGame.buffer[i] > 0)
            mapGame.buffer[i] -= 2;
        }
        if (mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] > 0)
          setGameOver(1);
        players[0].head = nextHead[0];
        mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] = players[0].length*2;
      }
  } else if (nextHeadSnake(players[0], &nextHead[0]) != -1 &&
             nextHeadSnake(players[1], &nextHead[1]) != -1 &&
             nextHead[0].X == nextHead[1].X && nextHead[0].Y == nextHead[1].Y) {
    setGameOver(3);
    players[0].head = nextHead[0];
    players[1].head = nextHead[1];
    if (mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] < 0) {
      players[0].length++;
      players[0].score += -mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)];
      players[1].length++;
      players[1].score += -mapGame.buffer[coord2int(nextHead[1].X, nextHead[1].Y, mapGame.size)];
      mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] = players[0].length*2 + players[1].length*2 - 1;
      setFood();
    } else {
      for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
        if (mapGame.buffer[i] == 1)
          mapGame.buffer[i] = 0;
        else if (mapGame.buffer[i] > 0)
          mapGame.buffer[i] -= 2;
      }
      mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] = players[0].length*2 + players[1].length*2 - 1;
    }
  } else {
    int temp = 0;
    if (nextHeadSnake(players[0], &nextHead[0]) == -1)
      setGameOver(2);
    else if (mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] < 0) {
      players[0].head = nextHead[0];
      players[0].length++;
      players[0].score += -mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)];
      mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] = players[0].length*2;
      setFood();
    } else {
      for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
        if (mapGame.buffer[i] > 0 && mapGame.buffer[i] % 2 == 0)
          mapGame.buffer[i] -= 2;
      }
      temp |= 1;
    }
    if (nextHeadSnake(players[1], &nextHead[1]) == -1)
      setGameOver(1);
    else if (mapGame.buffer[coord2int(nextHead[1].X, nextHead[1].Y, mapGame.size)] < 0) {
      players[1].head = nextHead[1];
      players[1].length++;
      players[1].score += -mapGame.buffer[coord2int(nextHead[1].X, nextHead[1].Y, mapGame.size)];
      mapGame.buffer[coord2int(nextHead[1].X, nextHead[1].Y, mapGame.size)] = players[1].length*2 - 1;
      setFood();
    } else {
      for (i = 0; i < ((int)mapGame.size.X)*((int)mapGame.size.Y); i++) {
        if (mapGame.buffer[i] == 1)
          mapGame.buffer[i] = 0;
        else if (mapGame.buffer[i] > 0 && mapGame.buffer[i] % 2 == 1)
          mapGame.buffer[i] -= 2;
      }
      temp |= 2;
    }
    if (temp & 1) {
      if (mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] > 0)
        setGameOver(2);
      players[0].head = nextHead[0];
      mapGame.buffer[coord2int(nextHead[0].X, nextHead[0].Y, mapGame.size)] = players[0].length*2;
    }
    if (temp & 2) {
      if (mapGame.buffer[coord2int(nextHead[1].X, nextHead[1].Y, mapGame.size)] > 0)
        setGameOver(1);
      players[1].head = nextHead[1];
      mapGame.buffer[coord2int(nextHead[1].X, nextHead[1].Y, mapGame.size)] = players[1].length*2 - 1;
    }
  }
}

void saveUncompletingGame() {
  if (!isGameOver()) {
    isHavingUncompletingGameVariable = 1;
    isPauseVariable = 1;
  }
}

void changeBetweenPlayPause() {
  isPauseVariable = 1-isPauseVariable;
}

void changeDirectionSnake(int player, Direction newDirection) {
  COORD tempNextHead;
  Snake tempSnake = players[player-1];
  tempSnake.direction = newDirection;
  if (nextHeadSnake(tempSnake, &tempNextHead) == -1 ||
      mapGame.buffer[coord2int(tempNextHead.X, tempNextHead.Y, mapGame.size)] != players[player-1].length*2 - ((player == 1) ? 2 : 3))
    players[player-1].direction = newDirection;
}

void loadHighScore() {
  int readHighScore = 0;
  FILE *f = fopen("highscore.bin", "rb");
  if (f != NULL) {
    fread(&readHighScore, sizeof(int), 1, f);
    fclose(f);
  }
  if (readHighScore > highScore)
    highScore = readHighScore;
}

void saveHighScore(int newHighScore) {
  if (highScore < newHighScore) {
    highScore = newHighScore;
    FILE *f = fopen("highscore.bin", "wb");
    if (f != NULL) {
      fwrite(&highScore, sizeof(int), 1, f);
      fclose(f);
    }
  }
}

//==================================================

void drawIntro(CHAR_INFO buffer[]) {
  int i;
  clearBuffer(buffer);
  drawTextToBuffer(buffer, 0, 0, "\xc9\xcd\xcd\xcd\xcd\xcd\xcdSNAKE GAME\xcd\xcd\xcd\xcd\xcd\xcd\xbb");
  i = 0;
  if (isHavingUncompletingGame()) {
    drawTextToBuffer(buffer, 0, i+1, "\xba %c Continue           \xba", ((currentSelection == i) ? '\xaf' : ' ')); i++;
  }
  drawTextToBuffer(buffer, 0, i+1, "\xba %c New Game           \xba", ((currentSelection == i) ? '\xaf' : ' ')); i++;
  drawTextToBuffer(buffer, 0, i+1, "\xba %c High Score         \xba", ((currentSelection == i) ? '\xaf' : ' ')); i++;
  drawTextToBuffer(buffer, 0, i+1, "\xba %c Credit             \xba", ((currentSelection == i) ? '\xaf' : ' ')); i++;
  if (!isHavingUncompletingGame()) {
    drawTextToBuffer(buffer, 0, i+1, "\xba                      \xba"); i++;
  }
  drawTextToBuffer(buffer, 0, i+1, "\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc");
}

void drawNewGame(CHAR_INFO buffer[]) {
  clearBuffer(buffer);
  drawTextToBuffer(buffer, 0, 0, "\xc9\xcd\xcd\xcd\xcd\xcd\xcdSNAKE GAME\xcd\xcd\xcd\xcd\xcd\xcd\xbb");
  drawTextToBuffer(buffer, 0, 1, "\xba %c 1 Player           \xba", ((currentSelection == 0) ? '\xaf' : ' '));
  drawTextToBuffer(buffer, 0, 2, "\xba %c 2 Player           \xba", ((currentSelection == 1) ? '\xaf' : ' '));
  drawTextToBuffer(buffer, 0, 3, "\xba                      \xba");
  drawTextToBuffer(buffer, 0, 4, "\xba                      \xba");
  drawTextToBuffer(buffer, 0, 5, "\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc");
}

void drawLevel(CHAR_INFO buffer[]) {
  clearBuffer(buffer);
  drawTextToBuffer(buffer, 0, 0, "\xc9\xcd\xcd\xcd\xcd\xcd\xcdSNAKE GAME\xcd\xcd\xcd\xcd\xcd\xcd\xbb");
  drawTextToBuffer(buffer, 0, 1, "\xba %c Easy               \xba", ((currentSelection == 0) ? '\xaf' : ' '));
  drawTextToBuffer(buffer, 0, 2, "\xba %c Medium             \xba", ((currentSelection == 1) ? '\xaf' : ' '));
  drawTextToBuffer(buffer, 0, 3, "\xba %c Hard               \xba", ((currentSelection == 2) ? '\xaf' : ' '));
  drawTextToBuffer(buffer, 0, 4, "\xba                      \xba");
  drawTextToBuffer(buffer, 0, 5, "\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc");
}

void drawHighScore(CHAR_INFO buffer[]) {
  clearBuffer(buffer);
  drawTextToBuffer(buffer, 0, 0, "\xc9\xcd\xcd\xcd\xcd\xcd\xcdSNAKE GAME\xcd\xcd\xcd\xcd\xcd\xcd\xbb");
  drawTextToBuffer(buffer, 0, 1, "\xba High Score           \xba");
  drawTextToBuffer(buffer, 0, 2, "\xba     %10d       \xba", highScore);
  drawTextToBuffer(buffer, 0, 3, "\xba                      \xba");
  drawTextToBuffer(buffer, 0, 4, "\xba                      \xba");
  drawTextToBuffer(buffer, 0, 5, "\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc");
}

void drawCredit(CHAR_INFO buffer[]) {
  clearBuffer(buffer);
  drawTextToBuffer(buffer, 0, 0, "\xc9\xcd\xcd\xcd\xcd\xcd\xcdSNAKE GAME\xcd\xcd\xcd\xcd\xcd\xcd\xbb");
  drawTextToBuffer(buffer, 0, 1, "\xba Credit               \xba");
  drawTextToBuffer(buffer, 0, 2, "\xba     Dang Diem Linh   \xba");
  drawTextToBuffer(buffer, 0, 3, "\xba     Duong Quang Vinh \xba");
  drawTextToBuffer(buffer, 0, 4, "\xba     Chu Quoc Minh    \xba");
  drawTextToBuffer(buffer, 0, 5, "\xc8\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xbc");
}

void drawInGame(CHAR_INFO buffer[]) {
  int i, j;
  COORD sizeScreen = getConsoleMaximumSize();
  WORD attribute = getConsoleScreenBufferInfo().wAttributes;
  if (!isPause() && !isGameOver())
    setNextFrameInGame();
  clearBuffer(buffer);
  for (i = 0; i < sizeScreen.X; i++) {
    buffer[coord2int(i, 0, sizeScreen)].Char.AsciiChar = '\xcd';
    buffer[coord2int(i, sizeScreen.Y-2, sizeScreen)].Char.AsciiChar = '\xcd';
  }
  for (i = 0; i < sizeScreen.Y-1; i++) {
    buffer[coord2int(0, i, sizeScreen)].Char.AsciiChar = '\xba';
    buffer[coord2int(sizeScreen.X-1, i, sizeScreen)].Char.AsciiChar = '\xba';
  }
  buffer[coord2int(0, 0, sizeScreen)].Char.AsciiChar = '\xc9';
  buffer[coord2int(sizeScreen.X-1, 0, sizeScreen)].Char.AsciiChar = '\xbb';
  buffer[coord2int(0, sizeScreen.Y-2, sizeScreen)].Char.AsciiChar = '\xc8';
  buffer[coord2int(sizeScreen.X-1, sizeScreen.Y-2, sizeScreen)].Char.AsciiChar = '\xbc';
  for (i = 0; i < mapGame.size.X; i++) {
    for (j = 0; j < mapGame.size.Y; j++) {
      int temp = mapGame.buffer[coord2int(i, j, mapGame.size)];
      if (temp < 0) {
        buffer[coord2int(i+1, j+1, sizeScreen)].Char.AsciiChar = '\xfe';
        buffer[coord2int(i+1, j+1, sizeScreen)].Attributes = changeForegroundColor(attribute, YELLOW, 1);
      } else if (temp == players[0].length*2 + players[1].length*2 - 1) {
        buffer[coord2int(i+1, j+1, sizeScreen)].Char.AsciiChar = '\xdb';
        buffer[coord2int(i+1, j+1, sizeScreen)].Attributes = changeForegroundColor(attribute, MAGENTA, 1);
      } else if (temp == players[0].length*2) {
        buffer[coord2int(i+1, j+1, sizeScreen)].Char.AsciiChar = '\xdb';
        buffer[coord2int(i+1, j+1, sizeScreen)].Attributes = changeForegroundColor(attribute, GREEN, 1);
      } else if (temp == players[1].length*2 - 1) {
        buffer[coord2int(i+1, j+1, sizeScreen)].Char.AsciiChar = '\xdb';
        buffer[coord2int(i+1, j+1, sizeScreen)].Attributes = changeForegroundColor(attribute, RED, 1);
      } else if (temp > 0) {
        buffer[coord2int(i+1, j+1, sizeScreen)].Char.AsciiChar = '\xdb';
        buffer[coord2int(i+1, j+1, sizeScreen)].Attributes = changeForegroundColor(attribute, ((temp%2) ? RED : GREEN), 0);
      }
    }
  }
  if (isGameOver()) {
    if (numberPlayers == 1) {
      drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Game Over! Your score is %d ...", players[0].score);
      for (i = 0; i < sizeScreen.X-1; i++)
        buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, WHITE, 1);
    } else {
      switch (getResultMatch()) {
        case PLAYER_1:
          drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Game Over! The winner is Player 1 ... Score: %d (P1) - %d (P2)",  players[0].score, players[1].score);
          for (i = 0; i < sizeScreen.X-1; i++)
            buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, GREEN, 1);
          break;
        case PLAYER_2:
          drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Game Over! The winner is Player 2 ... Score: %d (P1) - %d (P2)",  players[0].score, players[1].score);
          for (i = 0; i < sizeScreen.X-1; i++)
            buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, RED, 1);
          break;
        case TIE:
          drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Game Over! The result is tie ... Score: %d (P1) - %d (P2)",  players[0].score, players[1].score);
          for (i = 0; i < sizeScreen.X-1; i++)
            buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, WHITE, 1);
          break;
      }
    }
  } else if (isPause()) {
    drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Game Paused ...");
    for (i = 0; i < sizeScreen.X-1; i++)
      buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, WHITE, 1);
  } else if (numberPlayers == 1) {
    drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Score: %10d",  players[0].score);
    for (i = 0; i < sizeScreen.X-1; i++)
      buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, WHITE, 1);
    for (i = 0; i < 10; i++)
      buffer[coord2int(i+7, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, GREEN, 1);
  } else {
    drawTextToBuffer(buffer, 0, sizeScreen.Y-1, "Score: %10d - %10d",  players[0].score, players[1].score);
    for (i = 0; i < sizeScreen.X-1; i++)
      buffer[coord2int(i, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, WHITE, 1);
    for (i = 0; i < 10; i++)
      buffer[coord2int(i+7, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, GREEN, 1);
    for (i = 0; i < 10; i++)
      buffer[coord2int(i+20, sizeScreen.Y-1, sizeScreen)].Attributes = changeForegroundColor(attribute, RED, 1);
  }
}

//==================================================

void processKeyIntro(KEY_EVENT_RECORD event) {
  int numberSelection;
  if (event.bKeyDown)
    return;
  if (isHavingUncompletingGame())
    numberSelection = 4;
  else
    numberSelection = 3;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      isExitGameVariable = 1;
      break;
    case VK_RETURN:
      switch ((isHavingUncompletingGame()) ? currentSelection : currentSelection+1) {
        case 0:
          toContinue();
          break;
        case 1:
          toNewGame();
          break;
        case 2:
          toHighScore();
          break;
        case 3:
          toCredit();
          break;
      }
      break;
    case VK_UP:
      currentSelection = (currentSelection - 1 + numberSelection)%numberSelection;
      break;
    case VK_DOWN:
      currentSelection = (currentSelection + 1 + numberSelection)%numberSelection;
      break;
  }
}

void processKeyNewGame(KEY_EVENT_RECORD event) {
  int numberSelection = 2;
  if (event.bKeyDown)
    return;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      toIntro();
      break;
    case VK_RETURN:
      temporaryNumberPlayers = currentSelection + 1;
      toLevel();
      break;
    case VK_UP:
      currentSelection = (currentSelection - 1 + numberSelection)%numberSelection;
      break;
    case VK_DOWN:
      currentSelection = (currentSelection + 1 + numberSelection)%numberSelection;
      break;
  }
}

void processKeyLevel(KEY_EVENT_RECORD event) {
  int numberSelection = 3;
  if (event.bKeyDown)
    return;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      toNewGame();
      break;
    case VK_RETURN:
      switch (currentSelection) {
        case 0:
          setNewGame(temporaryNumberPlayers, EASY);
          break;
        case 1:
          setNewGame(temporaryNumberPlayers, MEDIUM);
          break;
        case 2:
          setNewGame(temporaryNumberPlayers, HARD);
          break;
      }
      toInGame();
      break;
    case VK_UP:
      currentSelection = (currentSelection - 1 + numberSelection)%numberSelection;
      break;
    case VK_DOWN:
      currentSelection = (currentSelection + 1 + numberSelection)%numberSelection;
      break;
  }
}

void processKeyHighScore(KEY_EVENT_RECORD event) {
  if (event.bKeyDown)
    return;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      toIntro();;
      break;
  }
}

void processKeyCredit(KEY_EVENT_RECORD event) {
  if (event.bKeyDown)
    return;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      toIntro();;
      break;
  }
}

void processKeyInGame(KEY_EVENT_RECORD event) {
  if (event.bKeyDown)
    return;
  switch (event.wVirtualKeyCode) {
    case VK_ESCAPE:
      if (!isGameOver())
        saveUncompletingGame();
      toIntro();;
      break;
    case VK_RETURN:
      changeBetweenPlayPause();
      break;
    case VK_UP:
      if (numberPlayers == 2)
        changeDirectionSnake(2, UP);
      else
        changeDirectionSnake(1, UP);
      break;
    case VK_DOWN:
      if (numberPlayers == 2)
        changeDirectionSnake(2, DOWN);
      else
        changeDirectionSnake(1, DOWN);
      break;
    case VK_LEFT:
      if (numberPlayers == 2)
        changeDirectionSnake(2, LEFT);
      else
        changeDirectionSnake(1, LEFT);
      break;
    case VK_RIGHT:
      if (numberPlayers == 2)
        changeDirectionSnake(2, RIGHT);
      else
        changeDirectionSnake(1, RIGHT);
      break;
    case 0x57:
      if (numberPlayers == 2)
        changeDirectionSnake(1, UP);
      break;
    case 0x53:
      if (numberPlayers == 2)
        changeDirectionSnake(1, DOWN);
      break;
    case 0x41:
      if (numberPlayers == 2)
        changeDirectionSnake(1, LEFT);
      break;
    case 0x44:
      if (numberPlayers == 2)
        changeDirectionSnake(1, RIGHT);
      break;
  }
}

//==================================================

void toIntro() {
  currentScene = INTRO;
  currentSelection = 0;
  addKeyEventFunction(&processKeyIntro);
}

void toContinue() {
  if (!isHavingUncompletingGame())
    toIntro();
  else
    toInGame();
}

void toNewGame() {
  currentScene = NEWGAME;
  currentSelection = 0;
  addKeyEventFunction(&processKeyNewGame);
}

void toLevel() {
  currentScene = LEVEL;
  currentSelection = 0;
  addKeyEventFunction(&processKeyLevel);
}

void toHighScore() {
  loadHighScore();
  currentScene = HIGHSCORE;
  addKeyEventFunction(&processKeyHighScore);
}

void toCredit() {
  currentScene = CREDIT;
  addKeyEventFunction(&processKeyCredit);
}

void toInGame() {
  currentScene = INGAME;
  addKeyEventFunction(&processKeyInGame);
}

//==================================================

void initGame() {
  initConsole();
  toIntro();
}
