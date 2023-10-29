//
// Created by akp on 28/10/23.
//

#include <fxcg/heap.h>
#include <fxcg/display.h>
#include "level.h"
#include "backend.h"
#include "sprites/grass_block.h"
#include "sprites/main_sprite.h"
#include "sprites/grass_block.h"

#define NULL 0

#define AIR 0
#define GRASS 1
#define BLOCK 2

color_t * getBlock(int code) {
  if (code == AIR) return NULL;
  if (code == GRASS) return grass_block;
  return NULL;
}

void initEntity(struct Entity * entity, int accx, int accy, int velx, int vely, int posx, int posy, enum EntityType type, unsigned short model[2048]) {
  entity->acc.x = accx;
  entity->acc.y = accy;
  entity->vel.x = velx;
  entity->vel.y = vely;
  entity->pos.x = posx;
  entity->pos.y = posy;
  entity->type = type;
  entity->next = NULL;
  for (int i = 0; i < 2048; i++) {
    entity->sprite[i] = model[i];
  }
}

void initBlock(struct Block * block, int accx, int accy, int velx, int vely, int posx, int posy, int width, int height, unsigned short tex[2048]) {
  block->acc.x = accx;
  block->acc.y = accy;
  block->vel.x = velx;
  block->vel.y = vely;
  block->pos.x = posx;
  block->pos.y = posy;
  block->width = width;
  block->height = height;
  for (int i = 0; i < 512; i++) {
    block->texture[i] = tex[i];
  }
}

struct Block * initLevel() {
  struct Block * level = NULL;
  struct Block * cur = level;
  int i = 0;
  for (int y = 0; y < level_1_height*16; y += 16) {
    for (int x = 0; x < level_1_width*16; x+= 16) {
      
      // If it's not air
      if (getBlock(level_1[i]) != NULL) {
        
        // If we haven't initialised the level, start
        if (cur == NULL) {
          level = (struct Block *) malloc(sizeof(struct Block));
          initBlock(level, 0, 0, 0, 0, x, y, 16, 16, getBlock(level_1[i]));
          cur = level;
        } else {
          struct Block * new_level = sys_malloc(sizeof(struct Block));
          initBlock(new_level, 0, 0, 0, 0, x, y, 16, 16, getBlock(level_1[i]));
          cur->next = new_level;
          cur = cur->next;
        }
      }
      
      i++;
    }
  }
  
  return level;
};

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

  int acc[] = {0, 0};
  int vel[] = {0, 0};
  int pos[] = {100, 100};

  initEntity(entities, acc, vel, pos, Player, main_sprite);

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
