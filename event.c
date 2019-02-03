#include "event.h"

//==================================================

static int isGetStdInputHandle = 0;
static HANDLE hStdInput;
HANDLE getStdInputHandle() {
  if (!isGetStdInputHandle) {
    isGetStdInputHandle = 1;
    hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  }
  return hStdInput;
}

//==================================================

static int isInitProcessing = 0;
static KEY_EVENT_FUNCTION_TYPE processKeyEvents;
static MOUSE_EVENT_FUNCTION_TYPE processMouseEvents;
void initProcessing() {
  if (!isInitProcessing) {
    isInitProcessing = 1;
    processKeyEvents = NULL;
    processMouseEvents = NULL;
  }
}

//==================================================

void addKeyEventFunction(KEY_EVENT_FUNCTION_TYPE funct) {
  if (!isInitProcessing) 
    initProcessing();
  processKeyEvents = funct;
}

void addMouseEventFunction(MOUSE_EVENT_FUNCTION_TYPE funct) {
  if (!isInitProcessing) 
    initProcessing();
  processMouseEvents = funct;
}

void removeKeyEventFunction() {
  if (!isInitProcessing) 
    initProcessing();
  processKeyEvents = NULL;
}

void removeMouseEventFunction() {
  if (!isInitProcessing) 
    initProcessing();
  processMouseEvents = NULL;
}

//==================================================

void processEventsNonBlocking() {
  if (!isInitProcessing) 
    initProcessing();
  DWORD nEvents, nEventsRead;
  INPUT_RECORD inputRecord;
  GetNumberOfConsoleInputEvents(getStdInputHandle(), &nEvents);
  while (nEvents) {
    ReadConsoleInput(getStdInputHandle(), &inputRecord, 1, &nEventsRead);
    nEvents -= nEventsRead;
    switch (inputRecord.EventType) {
      case KEY_EVENT:
        if (processKeyEvents != NULL)
          (*processKeyEvents)(inputRecord.Event.KeyEvent);
        break;
      case MOUSE_EVENT:
        if (processMouseEvents != NULL)
          (*processMouseEvents)(inputRecord.Event.MouseEvent);
        break;
      default:
        break; // Ignore other events
    }
  }
}
