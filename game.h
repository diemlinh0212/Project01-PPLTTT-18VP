#ifndef GAME_H
#define GAME_H

#include "load_lib.h"
#include "console.h"
#include "event.h"

// Definitions
typedef enum {INTRO, NEWGAME, LEVEL, HIGHSCORE, CREDIT, INGAME} Scene;
typedef enum {EASY, MEDIUM, HARD} Level;
typedef enum {UP, DOWN, LEFT, RIGHT} Direction;
typedef enum {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK} Color;
typedef enum {PLAYER_1, PLAYER_2, TIE, UNKNOWN} ResultMatch;
typedef struct {
  COORD head;
  Direction direction;
  int length;
  int score;
} Snake;
typedef struct {
  int buffer[65536];
  COORD size;
} Map;

// Utilities
WORD changeForegroundColor(WORD attributes, Color code, int isIntensity);
int nextHeadSnake(Snake player, COORD *nextHead);
int nextRandom();

// Get status variables
Scene getCurrentScene();
int getFoodValue();
ResultMatch getResultMatch();
int getSpeedSnake();
int isExitGame();
int isGameOver();
int isHavingUncompletingGame();
int isPause();

// Set status variables
void setNewMap();
void setFood();
void setNewGame(int nPlayers, Level level);
void setGameOver(int winnerPlayer);
void setNextFrameInGame();
void saveUncompletingGame();
void changeBetweenPlayPause();
void changeDirectionSnake(int player, Direction newDirection);
void loadHighScore();
void saveHighScore(int newHighScore);

// Draw scenes
void drawIntro(CHAR_INFO buffer[]);
void drawNewGame(CHAR_INFO buffer[]);
void drawLevel(CHAR_INFO buffer[]);
void drawHighScore(CHAR_INFO buffer[]);
void drawCredit(CHAR_INFO buffer[]);
void drawInGame(CHAR_INFO buffer[]);

// Key event functions
void processKeyIntro(KEY_EVENT_RECORD event);
void processKeyNewGame(KEY_EVENT_RECORD event);
void processKeyLevel(KEY_EVENT_RECORD event);
void processKeyHighScore(KEY_EVENT_RECORD event);
void processKeyCredit(KEY_EVENT_RECORD event);
void processKeyInGame(KEY_EVENT_RECORD event);

// Transfer scene
void toIntro();
void toContinue();
void toNewGame();
void toLevel();
void toHighScore();
void toCredit();
void toInGame();

// Init game
void initGame();

#endif

