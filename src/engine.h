#pragma once

#include <SDL2/SDL.h>
#include "definitions.h"

namespace Engine {
    extern int currentParticle;
    extern bool spawnParticles;

    // pack velocity into integers
    // first 16 bytes correspond to vertical velocity, last 16 correspond to horizontal
    // this sets a hard cap of velocity at (-2^15, 2^15-1) which theoretically will never be met regardless
    extern int velocity[GRID_WIDTH * GRID_HEIGHT];

    int GetVel(bool vertical, int xi, int yi);
    void SetVel(int xi, int yi, int xvel, int yvel);

    bool AttemptMove( int xi, int yi, int xoff, int yoff, int condition, int type);


    void HandleKeypress(SDL_KeyboardEvent e);
    bool Legal(int xi, int yi);
    void Loop();
}

#define GRAVITY 10
