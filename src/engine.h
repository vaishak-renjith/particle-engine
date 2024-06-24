#pragma once

#include <SDL2/SDL.h>
#include "definitions.h"

namespace Engine {
    extern int currentParticle;
    extern bool spawnParticles;

    extern bool positive;

    // pack velocity into integers
    // first 16 bytes correspond to vertical velocity, last 16 correspond to horizontal
    // this sets a hard cap of velocity at (-2^15, 2^15-1) which theoretically will never be met regardless
    extern int velocity[GRID_WIDTH * GRID_HEIGHT];

    int ClosestX(int x);
    int ClosestY(int Y);


    int GetVel(bool vertical, int xi, int yi);
    void SetVel(int xi, int yi, int xvel, int yvel);


    bool AttemptMove( int xi, int yi, int xoff, int yoff, int condition, int type);
    bool Legal(int xi, int yi);
    void Loop();


    void HandleKeypress(SDL_KeyboardEvent e);
}

#define GRAVITY 10
