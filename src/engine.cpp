#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include <cassert>
#include <iostream>

#include "engine.h"
#include "renderer.h"
#include "definitions.h"

bool Engine::spawnParticles = false;
int Engine::currentParticle = SAND;
int Engine::velocity[GRID_WIDTH * GRID_HEIGHT] = {0};


int Engine::GetVel(bool vertical, int xi, int yi) {
    int pos = xi + yi*GRID_WIDTH;
    int velPacked = velocity[pos];

    return (!vertical) ? (velPacked & 0x0000FFFF) : (velPacked >> 16);
}

void Engine::SetVel(int xi, int yi, int xvel, int yvel) {
    assert(xvel >= -pow(2, 15) && xvel <= pow(2, 15)-1 && yvel >= -pow(2, 15) && xvel <= pow(2, 15)-1);

    int pos = xi + yi*GRID_WIDTH;

    velocity[pos] = xvel<<16 | yvel;
}

void Engine::HandleKeypress(SDL_KeyboardEvent e) {
    switch (e.keysym.sym) {
        case SDLK_1:
            std::cout << "sand" << std::endl;
            currentParticle = SAND;
            break;
        case SDLK_2:
            std::cout << "water" << std::endl;
            currentParticle = WATER;
            break;
        case SDLK_BACKSPACE:
            std::cout << "void" << std::endl;
            currentParticle = VOID;
            break;
    }
}

bool Engine::Legal(int xi, int yi) {
    bool l = xi >= 0;
    bool r = xi < GRID_WIDTH;

    bool t = yi >= 0;
    bool b = yi < GRID_HEIGHT;

    return l && r && t && b;
}

bool Engine::AttemptMove(int xi, int yi, int xoff, int yoff, int condition, int type) {
    if (!Legal(xi+xoff, yi+yoff)) return false;

    int existingParticle = Renderer::GetPixelAt(Renderer::newPixels, xi+xoff, yi+yoff);

    if (existingParticle != condition) return false;

    // move particle
    Renderer::SetPixelAt(Renderer::newPixels, xi+xoff, yi+yoff, type);
    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);

    int xvel = GetVel(false, xi, yi);
    int yvel = GetVel(true, xi, yi);
    SetVel(xi+xoff, yi+yoff, xvel, yvel);
    SetVel(xi, yi, 0, 0);

    return true;
}

void Engine::Loop() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (spawnParticles) {
        // snap x to closest on-grid point
        int leftDist = mouseX%PIXEL_SIZE;

        if (leftDist < PIXEL_SIZE - leftDist) {
            mouseX -= leftDist;
        } else {
            mouseX += PIXEL_SIZE - leftDist;
        }

        // snap y to closest on-grid point
        int topDist = mouseY - (mouseY/PIXEL_SIZE*PIXEL_SIZE);

        if (topDist < PIXEL_SIZE - topDist) {
            mouseY -= topDist;
        } else {
            mouseY += PIXEL_SIZE - topDist;
        }

        // std::cout << "pixel Renderer::Set at " << x << " " << y << std::endl;
        Renderer::SetPixelAt(Renderer::ogPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, currentParticle);
        Renderer::SetPixelAt(Renderer::newPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, currentParticle);
        SetVel(mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, 0, GRAVITY);
    }


    for (int xi = GRID_WIDTH-1; xi >= 0; xi--) {
        for (int yi = GRID_HEIGHT-1; yi >= 0; yi--) {
            switch (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi)) {
              case WATER:
                if (AttemptMove(xi, yi, +0, +1, VOID, WATER)    || 
                    AttemptMove(xi, yi, -1, +1, VOID, WATER)    ||
                    AttemptMove(xi, yi, +1, +1, VOID, WATER)    ||
                    AttemptMove(xi, yi, +1, +0, VOID, WATER)    ||
                    AttemptMove(xi, yi, -1, +0, VOID, WATER))   {}

                break;

              case SAND:
                if (AttemptMove(xi, yi, +0, +1, VOID, SAND)    || 
                    AttemptMove(xi, yi, -1, +1, VOID, SAND)    ||
                    AttemptMove(xi, yi, +1, +1, VOID, SAND))   {}

                break;
            }
        }
    }
}
