//
// Created by akp on 28/10/23.
//
#include "backend.h"

#if defined(BACKEND_CALC)
int main()
#elif defined(BACKEND_PC)
int main(int argc, char** argv)
#endif
{
  // loadLevel(); // Read level file and load to screen

  // For calculator, things like removing top bar and stuff
  // For computer actually starting the screen window
  void * display;
  initDisplay(&display);

  struct Entity * entities;
  struct Block * level;
  unsigned short * keys;
  while (1) {
    pollEvents(keys); // Keys
    // moveEntities(); // Use keys and our values of velocity/acceleration
    // detectDeath(); // Depends on character position
    updateDisplay(level, entities, display);
  }
}