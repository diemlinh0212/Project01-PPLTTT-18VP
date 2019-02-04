#ifndef DEMO_H
#define DEMO_H

#include "load_lib.h"
#include "console.h"
#include "event.h"

void processKeyEventsInDemo(KEY_EVENT_RECORD event);
void processConsoleInDemo();
void initGameInDemo();
void nextFrameGameInDemo();
void nextFood();
void runDemo();

#endif
