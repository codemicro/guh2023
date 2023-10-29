#include "../sprites/main_sprite.h"
#include <fxcg/display.h>
#include <fxcg/heap.h>
#include <fxcg/keyboard.h>

#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 216

#define HOR_ACC 30
#define VER_ACC 15
#define HOR_DECEL 0.6
#define VERT_DECEL 1
#define GRAVITY 2
#define TIME_STEP 2

#define SCROLL_DIST 100

#define NULL 0

enum EntityType {
    Player,
    Enemy,
};

struct Coord {
    int x, y;
};

struct Pair {
    float x, y;
};

struct KeyNode {
    unsigned short key;
    struct KeyNode * next;
};

struct Entity {
    struct Coord pos;
    struct Pair vel, acc;
    enum EntityType type;
    const unsigned short sprite[2048];
    struct Entity * next;
};

struct Block {
    struct Coord pos;
    int width, height;
    struct Pair vel, acc;
    struct Block * next;
};

color_t rgbToColor_t(char red, char green, char blue) {
    color_t end = (((red & 0b11111000)<<8) + ((green & 0b11111100)<<3)+(blue>>3));
    return end;
}

int initDisplay(void ** display) {
    // Draws the main sprite
    // Setup display to clear the screen and draw
    // return the VRAM pointer to display?
    *display = GetVRAMAddress();
    return 1;
}

void clearKeyList(struct KeyNode * keys) {
    if (keys == NULL) {
        return;
    }

    struct KeyNode * prev = NULL;
    struct KeyNode * cur = keys;

    while (cur->next != NULL) {
        if (prev != NULL) {
            sys_free(prev);
        }
        prev = cur;
        cur = cur->next;
    }

    if (prev!= NULL) {
        sys_free(prev);
    }
    sys_free(cur);
}

void pollEvents(struct KeyNode ** keys) {
    int keycol, keyrow;
    unsigned short waste;
    // Not quite what I want -> makes it so that the program waits forever
    while (GetKeyWait_OS(&keycol, &keyrow, KEYWAIT_HALTOFF_TIMEROFF, 0, 0, &waste))
        {
            if (keycol == 3 && keyrow == 9) {
                if (*keys == NULL) {
                    *keys = sys_malloc(sizeof(struct KeyNode));
                    (*keys)->key = 0;
                    (*keys)->next = NULL;
                } else {
                    struct KeyNode * end_node;
                    for (end_node=*keys; end_node->next!=NULL; end_node=end_node->next);
                    struct KeyNode * new_node = sys_malloc(sizeof(struct KeyNode));
                    new_node->next = NULL;
                    new_node->key = 0;
                    end_node->next = new_node;
                }

            } else if (keycol == 2 && keyrow == 8) {
                if (*keys == NULL) {
                    *keys = sys_malloc(sizeof(struct KeyNode));
                    (*keys)->key = 1;
                    (*keys)->next = NULL;
                } else {
                    struct KeyNode * end_node;
                    for (end_node=*keys; end_node->next!=NULL; end_node=end_node->next);
                    struct KeyNode * new_node = sys_malloc(sizeof(struct KeyNode));
                    new_node->next = NULL;
                    new_node->key = 1;
                    end_node->next = new_node;
                }
            } else if (keycol == 2 && keyrow == 9) {
                if (*keys == NULL) {
                    *keys = sys_malloc(sizeof(struct KeyNode));
                    (*keys)->key = 2;
                    (*keys)->next = NULL;
                } else {
                    struct KeyNode * end_node;
                    for (end_node=*keys; end_node->next!=NULL; end_node=end_node->next);
                    struct KeyNode * new_node = sys_malloc(sizeof(struct KeyNode));
                    new_node->next = NULL;
                    new_node->key = 2;
                    end_node->next = new_node;
                }
            } else if (keycol == 9 && keyrow == 4) {
                // Do something to summon the menu
            }
        }
}

void updateDisplay(struct Block * level, struct Entity * entities, void * display)
{
    unsigned short * pixels = display;

    for (int y = 0; y < LCD_HEIGHT_PX; ++y)
    {
        for (int x = 0; x < LCD_WIDTH_PX; ++x)
        {
            pixels[x + y * LCD_WIDTH_PX] = rgbToColor_t(255, 255, 255);
        }
    }

    for (struct Block * block = level; block != NULL; block = block->next) {
        for (int i = block->pos.y; i < block->pos.y + block->height; ++i) {
            for (int j = block->pos.x; j < block->pos.x + block->width; ++j) {
                if (!(j < 0 || j >= LCD_WIDTH_PX || i < 0 || i >= LCD_HEIGHT_PX)) {
                    pixels[j + i * LCD_WIDTH_PX] = rgbToColor_t(255, 0, 0);
                }
            }
        }
    }

    for (struct Entity * entity = entities; entity != NULL; entity = entity->next) {
        for (int i = entity->pos.y; i < entity->pos.y + 32; ++i) {
            for (int j = entity->pos.x; j < entity->pos.x + 32; ++j) {
                if (!(j < 0 || j >= LCD_WIDTH_PX || i < 0 || i >= LCD_HEIGHT_PX)) {
                    pixels[j + i * LCD_WIDTH_PX] = rgbToColor_t(0, 255, 0);
                }
            }
        }
    }

    Bdisp_PutDisp_DD();
}

void moveEntities(struct KeyNode * keys, struct Entity * entities, struct Block * level) {
    struct KeyNode * key;
    struct Entity * cur_entity = entities;
    int done[] = {0, 0, 0};
    for (key = keys; key != NULL; key=key->next) {
        if (key->key < 3 && done[key->key]) {
            continue;
        }
        switch (key->key) {
            case 0:
                if (cur_entity->pos.x <= SCROLL_DIST) {
                    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
                        if (cur_entity->type != Player) {
                            cur_entity->acc.x += HOR_ACC;
                        }
                    }
                    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
                        cur_block->acc.x += HOR_ACC;
                    }
                } else {
                    cur_entity->acc.x -= HOR_ACC;
                }
                done[key->key] = 1;
                break;
            case 1:
                // If x >= 304, do not move player, move enemies
                if (cur_entity->pos.x >= LCD_WIDTH_PX-SCROLL_DIST) {
                    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
                        if (cur_entity->type != Player) {
                            cur_entity->acc.x -= HOR_ACC;
                        }
                    }
                    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
                        cur_block->acc.x -= HOR_ACC;
                    }
                } else {
                    cur_entity->acc.x += HOR_ACC;
                }
                done[key->key] = 1;
                break;
            case 2:
                // Change y acceleration
                cur_entity->acc.y = -VER_ACC;
                done[key->key] = 1;
                break;
            default:
                break;
        }
    }

    // Make player fall
    cur_entity = entities;

    // Decrease everyone's acceleration
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        if (cur_entity->acc.x >= 0.5 || -0.5 >= cur_entity->acc.x) {
            cur_entity->acc.x *= HOR_DECEL;
        } else {
            cur_entity->acc.x = 0;
        }

        if (cur_entity->type == Player) {
            if (cur_entity->acc.y < GRAVITY) {
                cur_entity->acc.y += VERT_DECEL;
            } else {
                cur_entity->acc.y = 0;
            }
        }
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        if (cur_block->acc.x >= 0.5 || -0.5 >= cur_block->acc.x) {
            cur_block->acc.x *= HOR_DECEL;
        } else {
            cur_block->acc.x = 0;
        }
    }

    // Get velocity from acceleration
    cur_entity = entities;
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        cur_entity->vel.x = cur_entity->acc.x/TIME_STEP;
        cur_entity->vel.y = cur_entity->acc.y/TIME_STEP;
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        cur_block->vel.x = cur_block->acc.x/TIME_STEP;
    }

    // Get position from velocity
    cur_entity = entities;
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        cur_entity->pos.x += cur_entity->vel.x;
        cur_entity->pos.y += cur_entity->vel.y;
    }

    cur_entity = entities;

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        cur_block->pos.x += cur_block->vel.x;
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
            // Entity boundaries
        int minX = cur_block->pos.x;
        int minY = cur_block->pos.y;
        int maxX = minX + cur_block->width;
        int maxY = minY + cur_block->height;

        // Check if player collided with them
        if (entities->pos.x + 32 > minX && entities->pos.x < maxX && entities->pos.y + 32 > minY && entities->pos.y < maxY) {
            entities->pos.y = minY - 32;
            break;
        }
    }

}

int detectDeath(struct Entity * entities) {

    // Get the player, first of all
    struct Entity * player = entities;
    struct Coord playerCoords = player->pos;

    // If the player is off the screen, die
    if (playerCoords.y > LCD_HEIGHT_PX) {
        return 1;
    }

    // Go through each entity
    struct Entity * entity = entities;
    while (entity != NULL) {
        
        // Check the type
        if (entity->type != Enemy) {
            entity = entity->next;
            continue;
        }

        // Entity boundaries
        int minX = entity->pos.x;
        int minY = entity->pos.y;
        int maxX = minX + 32;
        int maxY = minY + 32;

        // Check if player collided with them
        if (playerCoords.x > minX && playerCoords.x < maxX && playerCoords.y > minY && playerCoords.y < maxY) {
                return 1;
        }

        entity++;
    }

    return 0;
}

void CopySprite(color_t* sprite, int x, int y, int width, int height) {
    color_t* VRAM = (color_t*) GetVRAMAddress();
    VRAM += LCD_WIDTH_PX*y + x;
    for(int j=y; j<y+height; j++) {
        for(int i=x; i<x+width; i++) {
            *(VRAM++) = *(sprite++);
        }
        VRAM += LCD_WIDTH_PX-width;
    }
}