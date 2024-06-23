#pragma once

#include <SDL2/SDL.h>

namespace Engine {
    extern int currentParticle;
    extern bool spawnParticles;

    void HandleKeypress(SDL_KeyboardEvent e);
    bool Legal(int xi, int yi);
    void Loop();
}
