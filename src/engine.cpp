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


int Engine::ClosestX(int x) {
    int leftDist = x%PIXEL_SIZE;

    if (leftDist < PIXEL_SIZE - leftDist) {
        x -= leftDist;
    } else {
        x += PIXEL_SIZE - leftDist;
    }

    return x;
}

int Engine::ClosestY(int y) {
    int topDist = y - (y/PIXEL_SIZE*PIXEL_SIZE);

    if (topDist < PIXEL_SIZE - topDist) {
        y -= topDist;
    } else {
        y += PIXEL_SIZE - topDist;
    }

    return y;
}

int Engine::GetVel(bool vertical, int xi, int yi) {
    int pos = xi + yi*GRID_WIDTH;
    int velPacked = velocity[pos];

    return (!vertical) ? (velPacked & 0x0000FFFF) : (velPacked >> 16);
}

void Engine::SetVel(int xi, int yi, int xvel, int yvel) {
    assert(xvel >= -pow(2, 15) && xvel <= pow(2, 15)-1 && yvel >= -pow(2, 15) && xvel <= pow(2, 15)-1);

    int pos = xi + yi*GRID_WIDTH;

    velocity[pos] = yvel<<16 | xvel;
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
    // find most aggressive placement
    int aggXi = xi, aggYi = yi;

    std::cout << yi+yoff << std::endl;
    std::cout << yi << std::endl;

    for (int funcY = yi+yoff; funcY > yi; funcY--) {
        int funcX = (double)xoff/yoff * (funcY - yi) + xi;

        int closestX = ClosestX(funcX*PIXEL_SIZE)/PIXEL_SIZE;
        int closestY = ClosestY(funcY*PIXEL_SIZE)/PIXEL_SIZE;

        std::cout << "legal @ " << closestX << " " << closestY << std::endl;
        if (!Legal(closestX, closestY)) {
            if (funcY == yi+1) return false;
            continue;
        }
        std::cout << "condition" << std::endl;
        int existingParticle = Renderer::GetPixelAt(Renderer::newPixels, closestX, closestY);
        if (existingParticle != condition) continue;


        std::cout << "agg set" << std::endl;
        aggXi = closestX;
        aggYi = closestY;

        // skip if nothing is collided with
        if (funcY == yi+yoff) continue;
        // otherwise, kill horizontal movement
        SetVel(xi, yi, 0, GetVel(true, xi, yi));
    }

    // move particle
    Renderer::SetPixelAt(Renderer::newPixels, aggXi, aggYi, type);
    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);

    int xvel = GetVel(false, xi, yi);
    int yvel = GetVel(true, xi, yi);
    SetVel(aggXi, aggYi, xvel, yvel);
    SetVel(xi, yi, 0, 0);

    return true;
}

void Engine::Loop() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (spawnParticles) {
        // snap x to closest on-grid point
        mouseX = ClosestX(mouseX);
        mouseY = ClosestY(mouseY);

        // std::cout << "pixel Renderer::Set at " << x << " " << y << std::endl;
        Renderer::SetPixelAt(Renderer::ogPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, currentParticle);
        Renderer::SetPixelAt(Renderer::newPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, currentParticle);
        SetVel(mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, 0, GRAVITY);
    }


    for (int xi = GRID_WIDTH-1; xi >= 0; xi--) {
        for (int yi = GRID_HEIGHT-1; yi >= 0; yi--) {
            int xvel = GetVel(false, xi, yi);
            int yvel = GetVel(true, xi, yi);

            switch (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi)) {
              case WATER:
                if (xvel == 0) {
                if (AttemptMove(xi, yi, +0, yvel, VOID, WATER)    || 
                    AttemptMove(xi, yi, -1, yvel, VOID, WATER)    ||
                    AttemptMove(xi, yi, +1, yvel, VOID, WATER)    ||
                    AttemptMove(xi, yi, +1, 0, VOID, WATER)       ||
                    AttemptMove(xi, yi, -1, 0, VOID, WATER))      {}
                } else {
                    AttemptMove(xi, yi, xvel, yvel, VOID, WATER);
                }

                break;

              case SAND:
                if (xvel == 0) {
                if (AttemptMove(xi, yi, +0, yvel, VOID, SAND)    || 
                    AttemptMove(xi, yi, -1, yvel, VOID, SAND)    ||
                    AttemptMove(xi, yi, +1, yvel, VOID, SAND))   {}
                } else {
                    AttemptMove(xi, yi, xvel, yvel, VOID, SAND);
                }

                break;
            }
        }
    }
}
