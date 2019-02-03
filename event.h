#ifndef EVENT_H
#define EVENT_H

#include "load_lib.h"

typedef void (*KEY_EVENT_FUNCTION_TYPE)(KEY_EVENT_RECORD);
typedef void (*MOUSE_EVENT_FUNCTION_TYPE)(MOUSE_EVENT_RECORD);

// Get handles
HANDLE getStdInputHandle();

// Init processing
void initProcessing();

// Add and remove event function
void addKeyEventFunction(KEY_EVENT_FUNCTION_TYPE funct);
void addMouseEventFunction(MOUSE_EVENT_FUNCTION_TYPE funct);
void removeKeyEventFunction();
void removeMouseEventFunction();

// Process events without blocking
void processEventsNonBlocking();

#endif
