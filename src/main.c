//
// Created by akp on 28/10/23.
//
#include "backend.h"
#include <stdlib.h>

void initEntity(struct Entity * entity, int * acc, int * vel, int * pos, enum EntityType type) {
  entity->acc.x = acc[0];
  entity->acc.y = acc[1];
  entity->vel.x = vel[0];
  entity->vel.y = vel[1];
  entity->pos.x = pos[0];
  entity->pos.y = pos[1];
  entity->type = type;
  entity->next = NULL;
}

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
  struct KeyNode * keys = NULL;

  int acc[] = {0, 0};
  int vel[] = {0, 0};
  int pos[] = {100, 100};

  initEntity(entities, acc, vel, pos, Player);

  while (1) {
    pollEvents(&keys); // Keys
    moveEntities(keys, entities, level); // Use keys and our values of velocity/acceleration
    detectDeath(entities); // Depends on character position
    updateDisplay(level, entities, display);

    clearKeyList(keys);
    keys = NULL;
  }
}