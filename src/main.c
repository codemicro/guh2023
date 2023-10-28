//
// Created by akp on 28/10/23.
//
#include "backend.h"
#include <stdlib.h>

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

  struct Entity * entities = malloc(sizeof(struct Entity));
  struct Block * level = NULL;
  unsigned short * keys = NULL;

  while (1) {
    pollEvents(keys); // Keys
    moveEntities(keys, entities, level); // Use keys and our values of velocity/acceleration
    detectDeath(entities); // Depends on character position
    updateDisplay(level, entities, display);
  }
}