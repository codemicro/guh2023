#define BACKEND_PC 1

#ifndef BACKEND_H
#define BACKEND_H

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

int initDisplay(void ** display);
void clearKeyList(struct KeyNode * keys);
void pollEvents(struct KeyNode ** keys);
void moveEntities(struct KeyNode * keys, struct Entity * entities, struct Block * level);
int detectDeath(struct Entity * entities);
void updateDisplay(struct Block * level, struct Entity * entities, void * display);

#endif
