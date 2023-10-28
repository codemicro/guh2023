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
#define RIGHT SDLK_RIGHT
#define LEFT SDLK_LEFT
#define JUMP SDLK_UP

int initDisplay(void ** display){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LCD_WIDTH_PX, LCD_HEIGHT_PX,
        SDL_WINDOW_RESIZABLE);

    *display = window;
}


void pollEvents(unsigned short * keys) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
                switch( event.type ){
                    /* Keyboard event */
                    /* Pass the event data onto PrintKeyInfo() */
                    case SDL_KEYDOWN:
                        if (keys != NULL) {
                            switch (event.key.keysym.sym) {
                                case LEFT:
                                    *keys = 0;
                                    break;
                                case RIGHT:
                                    *keys = 1;
                                    break;
                                case JUMP:
                                    *keys = 2;
                                    break;
                                default:
                                    break;
                            }
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

    // Set every pixel to white.
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            pixels[x + y * width] =
                SDL_MapRGBA(window_surface->format, 255, 255, 255, 255);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void moveEntities(unsigned short * keys, struct Entity * entities, struct Block * level) {
    unsigned short * key;
    struct Entity * cur_entity = entities;
    for (key = keys; key != NULL; key++) {
        switch (*key) {
            case 0:
               // If x <= 80, do not move player, move enemies
                if (cur_entity->pos.x <= 80) {
                    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
                        if (cur_entity->type != Player) {
                            cur_entity->acc.x += 10;
                        }
                    }
                    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
                         cur_block->acc.x += 10;
                    }
                } else {
                    cur_entity->acc.x -= 10;
                }
                break;
            case 1:
                // If x >= 304, do not move player, move enemies
                if (cur_entity->pos.x >= 304) {
                    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
                        if (cur_entity->type != Player) {
                            cur_entity->acc.x -= 10;
                        }
                    }
                } else {
                    cur_entity->acc.x += 10;
                }
                break;
            case 2:
                // Change y acceleration
                cur_entity->acc.y -= 10;
                break;
        }
    }

    // Decrease everyone's acceleration
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        if (cur_entity->acc.x > 0.5) {
            cur_entity->acc.x *= 0.50;
        } else {
            cur_entity->acc.x = 0;
        }

        if (cur_entity->type == Player) {
            if (cur_entity->acc.y > 0.5) {
                cur_entity->acc.y *= 0.50;
            } else {
                cur_entity->acc.y = 0;
            }
        }
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        if (cur_block->acc.x > 0.5) {
            cur_block->acc.x *= 0.50;
        } else {
            cur_block->acc.x = 0;
        }
    }

    // Get velocity from acceleration
    cur_entity = entities;
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        cur_entity->vel.x += cur_entity->acc.x/10;
        cur_entity->vel.y += cur_entity->acc.y/10;
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        cur_block->vel.x += cur_block->acc.x/10;
    }

    // Get position from velocity
    cur_entity = entities;
    for (cur_entity = entities; cur_entity != NULL; cur_entity=cur_entity->next) {
        cur_entity->pos.x += floor(cur_entity->vel.x);
        cur_entity->pos.y += floor(cur_entity->vel.y);
    }

    for (struct Block * cur_block = level; cur_block != NULL; cur_block=cur_block->next) {
        cur_block->vel.x += floor(cur_block->vel.x);
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
