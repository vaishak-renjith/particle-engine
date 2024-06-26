#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include <cassert>
#include <iostream>

#include "engine.h"
#include "renderer.h"
#include "definitions.h"

bool Engine::spawnParticles = false;
bool Engine::positive = false;
bool Engine::pause = false;
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

    int vel = (vertical) ? (velPacked>>16) : velPacked&0x0000FFFF;
    if ((vel & 0x00008000) != 0) {
        vel |= 0xFFFF0000;
    }
    return vel;
}

void Engine::SetVel(int xi, int yi, int xvel, int yvel) {
    assert(xvel >= -pow(2, 15) && xvel <= pow(2, 15)-1 && yvel >= -pow(2, 15) && xvel <= pow(2, 15)-1);

    int pos = xi + yi*GRID_WIDTH;

    velocity[pos] = yvel<<16 | (xvel&0x0000FFFF);
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

        case SDLK_EQUALS:
            positive = true;
            break;
        case SDLK_MINUS:
            positive = false;
            break;

        case SDLK_p:
            pause = !pause;
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


void FindMostAggressiveMove(int &aggXi, int &aggYi, bool isHorizontalSearch,
                            int xi, int yi, int xoff, int yoff, int condition) {
    int mostIdx = (isHorizontalSearch) ? (xi+xoff) : yi+yoff;
    int leastIdx = (isHorizontalSearch) ? xi : yi;
    
    int funcX, funcY;
    int closestX, closestY;
    int existingParticle;

    // std::cout << "mostidx " << mostIdx << std::endl;
    // std::cout << "leastidx " << leastIdx << std::endl;


    if (mostIdx > leastIdx) {
        for (; mostIdx > leastIdx; mostIdx--) {
            run_checks:
            funcX = mostIdx, funcY = mostIdx;

            if (isHorizontalSearch)
                funcY = (double)yoff/xoff * (funcX - xi) + yi;
            else
                funcX = (double)xoff/yoff * (funcY - yi) + xi;

            closestX = Engine::ClosestX(funcX*PIXEL_SIZE)/PIXEL_SIZE;
            closestY = Engine::ClosestY(funcY*PIXEL_SIZE)/PIXEL_SIZE;
            // std::cout << "fail legal" << std::endl;
            if (!Engine::Legal(closestX, closestY)) continue;


            // std::cout << "fail condition" << std::endl;
            existingParticle = Renderer::GetPixelAt(Renderer::newPixels, closestX, closestY);
            if (existingParticle != condition) continue;


            // std::cout << "aggx/y: " << aggXi << " " << aggYi << std::endl;

            // std::cout << "fail pow" << std::endl;
            if (std::pow(closestX-xi, 2) + std::pow(closestY-yi, 2) > std::pow(aggXi-xi, 2) + std::pow(aggYi-yi, 2)) {
                // std::cout << "ix/y: " << xi << " " << yi << std::endl;
                // std::cout << "cx/y: " << closestX << " " << closestY << std::endl;

                aggXi = closestX;
                aggYi = closestY;
            }
            // std::cout << "break" << std::endl;
            break;
        }
    } else {
        for (; mostIdx < leastIdx; mostIdx++) {
            goto run_checks;
        }
    }
}

bool Engine::AttemptMove(int xi, int yi, int xoff, int yoff, int condition, int type, bool pure) {
    // find most aggressive placement
    int aggXi = xi, aggYi = yi;
    if (!pure) {
        FindMostAggressiveMove(aggXi, aggYi, true, xi, yi, xoff, yoff, condition);
        FindMostAggressiveMove(aggXi, aggYi, false, xi, yi, xoff, yoff, condition);

        // if most aggressive position is starting position, try something else
        if (aggXi == xi && aggYi == yi) return false;
        // skip if nothing is collided with
        if (!(xi+xoff == aggXi && yi+yoff == aggYi)) {
            if (!Legal(xi+xoff, yi+yoff)) { // if boundary hit, then set velocity to 0
                SetVel(xi, yi, 0, yoff);
            } else { // otherwise, swap velocities
                int xvel = GetVel(false, xi, yi);
                int yvel = GetVel(true, xi, yi);

                // this needs to be updated when i eventually make it so that skipping pixels is impossible
                int xcvel = GetVel(false, xi+xoff, yi+yoff);
                int ycvel = GetVel(true, xi+xoff, yi+yoff);

                SetVel(xi, yi, xcvel, ycvel);
                SetVel(xi+xoff, yi+yoff, xvel, yvel);
            }
        }
    } else {
        if (!Legal(xi+xoff, yi+yoff)) return false;
        if (Renderer::GetPixelAt(Renderer::newPixels, xi+xoff, yi+yoff) != condition) return false;

        aggXi = xi+xoff;
        aggYi = yi+yoff;
    }

    // move particle
    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);
    Renderer::SetPixelAt(Renderer::newPixels, aggXi, aggYi, type);

    int xvel = GetVel(false, xi, yi);
    int yvel = GetVel(true, xi, yi);
    SetVel(aggXi, aggYi, xvel, yvel);
    SetVel(xi, yi, 0, 0);

    return true;
}

bool Engine::AttemptMove(int xi, int yi, int xoff, int yoff, int condition, int type) {
    return AttemptMove(xi, yi, xoff, yoff, condition, type, false);
}

void Engine::Loop() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    mouseX = std::max(0, mouseX);
    mouseX = std::min(mouseX, SCREEN_WIDTH-1);
    mouseY = std::max(0, mouseY);
    mouseY = std::min(mouseY, SCREEN_HEIGHT-1);

    for (int xi = GRID_WIDTH-1; xi >= 0 && !pause; xi--) {
        for (int yi = GRID_HEIGHT-1; yi >= 0; yi--) {
            int xvel = GetVel(false, xi, yi);
            int yvel = GetVel(true, xi, yi);

            switch (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi)) {
              case WATER:
                  // std::cout << "xvel: " << xvel << std::endl;
                  // std::cout << "yvel: " << yvel << std::endl;
                  if (xvel != 0) {
                      AttemptMove(xi, yi, xvel, yvel, VOID, WATER);
                  }
                  else if (Legal(xi, yi+1) && Renderer::GetPixelAt(Renderer::ogPixels, xi, yi+1) == VOID && (
                      AttemptMove(xi, yi, 0, yvel, VOID, WATER)
                  )) {}
                  else if (Legal(xi, yi+1) && Renderer::GetPixelAt(Renderer::ogPixels, xi, yi+1) == WATER && (
                      AttemptMove(xi, yi, -yvel, 0, VOID, WATER) ||
                      AttemptMove(xi, yi, +yvel, 0, VOID, WATER)
                  )) {}
                  else if (
                      AttemptMove(xi, yi, +1, yvel, VOID, WATER, true) ||
                      AttemptMove(xi, yi, -1, yvel, VOID, WATER, true) ||
                      AttemptMove(xi, yi, +0, yvel, VOID, WATER)       ||
                      AttemptMove(xi, yi, -yvel, yvel, VOID, WATER)    ||
                      AttemptMove(xi, yi, +yvel, yvel, VOID, WATER)
                  ) {}

                break;

              case SAND:
                if (xvel != 0) {
                    AttemptMove(xi, yi, xvel, yvel, VOID, SAND);
                }
                else if (
                    AttemptMove(xi, yi, +1, yvel, VOID, SAND, true) ||
                    AttemptMove(xi, yi, -1, yvel, VOID, SAND, true) ||
                    AttemptMove(xi, yi, +0, yvel, VOID, SAND)       ||
                    AttemptMove(xi, yi, -yvel, yvel, VOID, SAND)    ||
                    AttemptMove(xi, yi, +yvel, yvel, VOID, SAND)
                ) {}

                break;
            }
        }
    }

    if (spawnParticles) {
        // snap x to closest on-grid point
        mouseX = ClosestX(mouseX);
        mouseY = ClosestY(mouseY);

        if (Renderer::GetPixelAt(Renderer::ogPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE) != VOID) {
            int xi = mouseX/PIXEL_SIZE;
            int yi = mouseY/PIXEL_SIZE;

            int xvel = GetVel(false, xi, yi);
            int yvel = GetVel(true, xi, yi);

            std::cout << "debug (" << xi << ", " << yi << ")" << std::endl;
            std::cout << "veloc (" << xvel << ", " << yvel << ")" << std::endl;
        }

        else {
            for (int xb = -BRUSH_RAD; xb <= BRUSH_RAD; xb++) {
                for (int yb = -BRUSH_RAD; yb <= BRUSH_RAD; yb++) {
                    if (pow(xb, 2) + pow(yb, 2) > pow(BRUSH_RAD, 2)) continue;

                    Renderer::SetPixelAt(Renderer::ogPixels, mouseX/PIXEL_SIZE + xb, mouseY/PIXEL_SIZE + yb, currentParticle);
                    Renderer::SetPixelAt(Renderer::newPixels, mouseX/PIXEL_SIZE + xb, mouseY/PIXEL_SIZE + yb, currentParticle);
                    SetVel(mouseX/PIXEL_SIZE + xb, mouseY/PIXEL_SIZE + yb, rand()%5*((positive)?1:-1), GRAVITY);
                }
            }
        }
    }
}
