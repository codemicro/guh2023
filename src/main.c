//
// Created by akp on 28/10/23.
//
#include "level.h"
#include "backend.h"
#include "sprites/grass_block.h"
#include <fxcg/heap.h>

#define NULL 0

#define AIR 0
#define GRASS 1
#define BLOCK 2

struct Block * initLevel() {
  
  int[] levelRaw = level_1;
  int array_size = sizeof(levelRaw) / sizeof(int);
  
  Block * level = sys_malloc(array_size * sizeof(struct Block));
  int i = 0;
  for (int x = 0; x < level_1_width; x += 20) {
    for (int y = 0; y < level_1_height; y += 20) {
      
      initBlock(level, 0, 0, {x, y}, 20, 20, getBlock(i));
      level++;
      i++;
    }
  }
  
  return level;
}

color_t * getBlock(int code) {
  if (code == AIR) return {};
  if (code == GRASS) return grass_block;
  return {};
}

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

void initBlock(struct Block * block, int * acc, int * vel, int * pos, int width, int height, color_t * colour) {
  block->acc.x = acc[0];
  block->acc.y = acc[1];
  block->vel.x = vel[0];
  block->vel.y = vel[1];
  block->pos.x = pos[0];
  block->pos.y = pos[1];
  block->width = width;
  block->height = height;
  block->sprite = colour;
  block->next = NULL;
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
  struct Block * level = initLevel();
  struct KeyNode * keys = NULL;

  // loadLevel(level); // Read level file and load to screen

  int acc[] = {0, 0};
  int vel[] = {0, 0};
  int pos[] = {100, 100};

  initEntity(entities, acc, vel, pos, Player);

  int blockpos[] = {0, 180};
  initBlock(level, acc, vel, blockpos, 384, 20);

  int dead = 0;
  while (1) {
    pollEvents(&keys); // Keys
    moveEntities(keys, entities, level); // Use keys and our values of velocity/acceleration
    dead = detectDeath(entities); // Depends on character position
    updateDisplay(level, entities, display);

    clearKeyList(keys);
    keys = NULL;

    if (dead) {
      return 0;
    }
  }
  return 0;
}
