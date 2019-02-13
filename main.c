#include "load_lib.h"
#include "console.h"
#include "event.h"
#include "game.h"

int main() {
  CHAR_INFO buffer[65536];
  initGame();
  while (!isExitGame()) {
    delayFPS(60);
    processEventsNonBlocking();
    switch (getCurrentScene()) {
      case INTRO:
        drawIntro(buffer);
        writeFullConsoleBuffer(buffer);
        break;
      case NEWGAME:
        drawNewGame(buffer);
        writeFullConsoleBuffer(buffer);
        break;
      case LEVEL:
        drawLevel(buffer);
        writeFullConsoleBuffer(buffer);
        break;
      case HIGHSCORE:
        drawHighScore(buffer);
        writeFullConsoleBuffer(buffer);
        break;
      case CREDIT:
        drawCredit(buffer);
        writeFullConsoleBuffer(buffer);
        break;
      case INGAME:
        drawInGame(buffer);
        writeFullConsoleBuffer(buffer);
        break;
    }
  }
  return 0;
}
