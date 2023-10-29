//
// Created by akp on 28/10/23.
//
#include "backend.h"
#include <fxcg/heap.h>
#include "sprites/main_sprite.h"
#include "sprites/grass_block.h"

#define NULL 0

void initEntity(struct Entity * entity, int * acc, int * vel, int * pos, enum EntityType type, unsigned short model[2048]) {
  entity->acc.x = acc[0];
  entity->acc.y = acc[1];
  entity->vel.x = vel[0];
  entity->vel.y = vel[1];
  entity->pos.x = pos[0];
  entity->pos.y = pos[1];
  entity->type = type;
  entity->next = NULL;
  for (int i = 0; i < 2048; i++) {
    entity->sprite[i] = model[i];
  }
}

void initBlock(struct Block * block, int * acc, int * vel, int * pos, int width, int height, unsigned short tex[2048]) {
  block->acc.x = acc[0];
  block->acc.y = acc[1];
  block->vel.x = vel[0];
  block->vel.y = vel[1];
  block->pos.x = pos[0];
  block->pos.y = pos[1];
  block->width = width;
  block->height = height;
  block->next = NULL;
  for (int i = 0; i < 512; i++) {
    block->texture[i] = tex[i];
  }
}

#if defined(BACKEND_CALC)
int main()
#elif defined(BACKEND_PC)
int main(int argc, char** argv)
#endif
{
  // For calculator, things like removing top bar and stuff
  // For computer actually starting the screen window
  unsigned short * display;
  initDisplay((void *)&display);

  struct Entity * entities = sys_malloc(sizeof(struct Entity));
  struct Block * level = sys_malloc(sizeof(struct Block));
  struct KeyNode * keys = NULL;

  // loadLevel(level); // Read level file and load to screen

  int acc[] = {0, 0};
  int vel[] = {0, 0};
  int pos[] = {100, 100};

  initEntity(entities, acc, vel, pos, Player, main_sprite);

  int blockpos[] = {0, 180};
  initBlock(level, acc, vel, blockpos, 384, 16, grass_block);

  int dead = 0;
  int menu = 0;
  int jumpFlag = 1;
  while (1) {
    menu = pollEvents(&keys); // Keys
    moveEntities(keys, entities, level, &jumpFlag); // Use keys and our values of velocity/acceleration
    dead = detectDeath(entities); // Depends on character position
    updateDisplay(level, entities, display);

    clearKeyList(keys);
    keys = NULL;

    if (dead || menu) {
      return 0;
    }
  }
  return 0;
}
