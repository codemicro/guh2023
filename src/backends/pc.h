#ifndef PC_H
#define PC_H

enum EntityType {
    Player,
    Enemy,
};

struct Coord {
    int x, y;
};

struct Entity {
    struct Coord pos;
    float vel, acc;
    enum EntityType type;
    const unsigned short sprite[2048];
};

struct Block {
    struct Coord pos;
    int width, height;
};

int initDisplay(void ** display);
void pollEvents(unsigned short * keys);
void moveEntities(unsigned short key, struct Entity * entities);
int detectDeath(struct Entity * entities);
void updateDisplay(struct Block * level, struct Entity * entities, void * display);

#endif