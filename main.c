#include "load_lib.h"
#include "console.h"
#include "event.h"
#include "demo.h"

#define DEMO_MODE

int main() {
#ifdef DEMO_MODE
  runDemo();
#endif
  return 0;
}
