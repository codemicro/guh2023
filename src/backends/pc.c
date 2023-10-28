// void *GetVRAMAddress (Can ignore as just obtains the address of VRAM in memory)
// void Bdisp_PutDisp_DD (Puts VRAM to the display and swaps the screens (uses double buffering))
// int GetKeyWait_OS(int* column, int* row, int type_of_waiting, int timeout_period, int menu, unsigned short* keycode)
// int GetKey(int* key) (blocking function)
// void PrintXY(int x, int y, char* message, int mode, int color)

#include <stdio.h>
#include <SDL2/SDL.h>
#include "pc.h"

int initDisplay(void ** display){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_RESIZABLE);

    *display = window;
}


void pollEvents(unsigned short * keys) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(0);

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

void moveEntities(unsigned short key, struct Entity * entities) {
    // do thingies
}

int detectDeath(struct Entity * entities) {
    // any entity of enemy hitting player
    // any instance of player off the screen
}
