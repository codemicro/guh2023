#include <fxcg/heap.h>
#include <fxcg/display.h>
#include "level.h"
#include "backend.h"
#include "sprites/grass_block.h"
#include "sprites/dirt_block.h"
#include "sprites/main_sprite.h"

#define NULL 0

color_t * getBlock(int code) {
  switch (code) {
    case Air:
      return NULL;
    case Grass:
      return grass_block;
    case Dirt:
      return dirt_block;
    default:
      return NULL;
  }
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

void initBlock(struct Block * block, int accx, int accy, int velx, int vely, int posx, int posy, int width, int height, unsigned short tex[512]) {
  block->acc.x = accx;
  block->acc.y = accy;
  block->vel.x = velx;
  block->vel.y = vely;
  block->pos.x = posx;
  block->pos.y = posy;
  block->width = width;
  block->height = height;
  block->next = NULL;
  for (int i = 0; i < 512; i++) {
    block->texture[i] = tex[i];
  }
}

struct Block * initLevel() {
  struct Block * level = NULL;
  struct Block * cur = level;
  int i = 0;
  for (int y = 0; y < level_1_height*16; y += 16) {
    for (int x = 0; x < level_1_width*16; x += 16) {
      if (getBlock(level_1[i]) != NULL) {
        if (level == NULL) {
            level = (struct Block *)sys_malloc(sizeof(struct Block));
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

  for (struct Block * curBlock = level; curBlock != NULL; curBlock = curBlock->next) {
    if (curBlock->texture == dirt_block) {
      PrintXY(3, 3, "husdigfhsd", 0, 0);
      Bdisp_PutDisp_DD();
    }
  }

  initEntity(entities, 0, 0, 0, 0, 100, 100, Player, main_sprite);

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
