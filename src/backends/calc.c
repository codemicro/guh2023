
#include "../sprites/main_sprite.h"
#include <fxcg/display.h>

#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 216

#define SPAWNING_Y 180

#define JUMP 0x0902
#define LEFT 0x0903
#define RIGHT 0x0802

extern color_t* main_sprite;
// extern color_t* grass_block;
// extern color_t* dirt_block;
// extern color_t* enemy_sprite;

// char* VRAM = (char*) 0xA8000000;

enum EntityType {
    Player,
    Entity
};

struct Coord {
    int x, y;
};

struct Entity {
    struct Coord pos;
    float vel, acc;
    enum EntityType type;
    const color_t * sprite;
};

int initDisplay(void * display) {
    
    // Draws the main sprite
    Entity main_sprite_entity = {{10, SPAWNING_Y}, 0, 0, Player, main_sprite};
    CopySprite(main_sprite_entity.sprite);
}

void pollEvents(unsigned short * keys) {
    
}

void moveEntities(unsigned short key, struct Entity * entities) {
    
}

int detectDeath(struct Entity * entities) {

}

void updateDisplay(struct Block * level, struct Entity * entities, void * display) {
    
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
