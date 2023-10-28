// void *GetVRAMAddress (Can ignore as just obtains the address of VRAM in memory)
// void Bdisp_PutDisp_DD (Puts VRAM to the display and swaps the screens (uses double buffering))
// int GetKeyWait_OS(int* column, int* row, int type_of_waiting, int timeout_period, int menu, unsigned short* keycode)
// int GetKey(int* key) (blocking function)
// void PrintXY(int x, int y, char* message, int mode, int color)

#include <stdio.h>
#include <SDL2/SDL.h>
#include "pc.h"

#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 216

#define HOR_ACC 10
#define VER_ACC 10
#define HOR_DECEL 0.5
#define VER_DECEL 0.5
#define TIME_STEP 4


int initDisplay(void ** display){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LCD_WIDTH_PX, LCD_HEIGHT_PX,
        SDL_WINDOW_RESIZABLE);

    *display = window;
}

void clearKeyList(struct KeyNode * keys) {
    if (keys == NULL) {
        return;
    }

    struct KeyNode * prev = NULL;
    struct KeyNode * cur = keys;

    while (cur->next != NULL) {
        if (prev != NULL) {
            free(prev);
        }
        prev = cur;
        cur = cur->next;
    }

    if (prev!= NULL) {
        free(prev);
    }
    free(cur);
}

void pollEvents(struct KeyNode ** keys) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
                switch( event.type ){
                    /* Keyboard event */
                    /* Pass the event data onto PrintKeyInfo() */
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT:
                                if (*keys == NULL) {
                                    *keys = malloc(sizeof(struct KeyNode));
                                    (*keys)->key = 0;
                                    (*keys)->next = NULL;
                                } else {
                                    struct KeyNode * end_node;
                                    for (end_node=*keys; end_node->next!=NULL; end_node=end_node->next);
                                    struct KeyNode * new_node = malloc(sizeof(struct KeyNode));
                                    new_node->next = NULL;
                                    new_node->key = 0;
                                    end_node->next = new_node;
                                }
                                break;
                            case SDLK_RIGHT:
                                if (*keys == NULL) {
                                    *keys = malloc(sizeof(struct KeyNode));
                                    (*keys)->key = 1;
                                    (*keys)->next = NULL;
                                } else {
                                    struct KeyNode * end_node;
                                    for (end_node=*keys; end_node->next!=NULL; end_node=end_node->next);
                                    struct KeyNode * new_node = malloc(sizeof(struct KeyNode));
                                    new_node->next = NULL;
                                    new_node->key = 1;
                                    end_node->next = new_node;
                                }
                                break;
                            case SDLK_UP:
                                if (*keys == NULL) {
                                    *keys = malloc(sizeof(struct KeyNode));
                                    (*keys)->key = 2;
                                    (*keys)->next = NULL;
                                } else {
                                    struct KeyNode * end_node;
                                    for (end_node=*keys; end_node->next!=NULL; end_node=end_node->next);
                                    struct KeyNode * new_node = malloc(sizeof(struct KeyNode));
                                    new_node->next = NULL;
                                    new_node->key = 2;
                                    end_node->next = new_node;
                                }
                                break;
                            default:
                                break;
                            }
                        break;

                    /* SDL_QUIT event (window close) */
                    case SDL_QUIT:
                        exit(0);
                        break;

                    default:
                        break;
                }
        }
}

void updateDisplay(struct Block * level, struct Entity * entities, void * display)
{
    SDL_Window * window = ((SDL_Window *) display);
    SDL_Surface * window_surface = SDL_GetWindowSurface(window);
    int height = window_surface->h;
    int width = window_surface->w;

    unsigned int * pixels = window_surface->pixels;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            pixels[x + y * width] =
                SDL_MapRGBA(window_surface->format, 255, 255, 255, 255);
        }
    }

    for (struct Block * block = level; block != NULL; block = block->next) {
        for (int i = block->pos.y; i < block->pos.y + block->height; ++i) {
            for (int j = block->pos.x; j < block->pos.x + block->width; ++j) {
                pixels[j + i * width] = 
                    SDL_MapRGBA(window_surface->format, 255, 0, 0, 255);
            }
        }
    }

    for (struct Entity * entity = entities; entity != NULL; entity = entity->next) {
        for (int i = entity->pos.y; i < entity->pos.y + 32; ++i) {
            for (int j = entity->pos.x; j < entity->pos.x + 32; ++j) {
                pixels[j + i * width] = 
                    SDL_MapRGBA(window_surface->format, 0, 255, 0, 255);
            }
        }
    }

    SDL_UpdateWindowSurface(window);
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
               // If x <= 80, do not move player, move enemies
                if (cur_entity->pos.x <= 80) {
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
                if (cur_entity->pos.x >= 304) {
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
                cur_entity->acc.y -= VER_ACC;
                done[key->key] = 1;
                break;
            default:
                break;
        }
    }

    // Decrease everyone's acceleration
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        if (cur_entity->acc.x >= 0.5 || -0.5 >= cur_entity->acc.x) {
            cur_entity->acc.x *= HOR_DECEL;
        } else {
            cur_entity->acc.x = 0;
        }

        if (cur_entity->type == Player) {
            if (cur_entity->acc.y >= 0.5 || -0.5 >= cur_entity->acc.y) {
                cur_entity->acc.y *= VER_DECEL;
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
        cur_entity->pos.x += floor(cur_entity->vel.x);
        cur_entity->pos.y += floor(cur_entity->vel.y);
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        cur_block->pos.x += floor(cur_block->vel.x);
    }
}

int detectDeath(struct Entity * entities) {

    // Get the player, first of all
    struct Entity * player = entities;
    struct Coord playerCoords = player->pos;

    // If the player is off the screen, die
    if (playerCoords.x < 0 || playerCoords.x > LCD_WIDTH_PX || playerCoords.y < 0 || playerCoords.y > LCD_HEIGHT_PX) {
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
