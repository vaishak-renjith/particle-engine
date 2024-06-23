#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include "engine.h"
#include "renderer.h"
#include "definitions.h"

#include <iostream>

bool Engine::spawnParticles = false;
int Engine::currentParticle = SAND;


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
    }


    for (int xi = GRID_WIDTH-1; xi >= 0; xi--) {
        for (int yi = GRID_HEIGHT-1; yi >= 0; yi--) {
            switch (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi)) {
              case WATER:
                Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);

                if (Legal(xi, yi+1) && Renderer::GetPixelAt(Renderer::newPixels, xi, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi+1, WATER);

                else if (Legal(xi-1, yi+1) && Renderer::GetPixelAt(Renderer::newPixels, xi-1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi-1, yi+1, WATER);

                else if (Legal(xi+1, yi+1) && Renderer::GetPixelAt(Renderer::newPixels, xi+1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi+1, yi+1, WATER);

                else if (Legal(xi+1, yi) && Renderer::GetPixelAt(Renderer::newPixels, xi+1, yi) == VOID) {
                    std::cout << "void at " << xi << " " << yi << std::endl;
                    Renderer::SetPixelAt(Renderer::newPixels, xi+1, yi, WATER);
                }

                else if (Legal(xi-1, yi) && Renderer::GetPixelAt(Renderer::newPixels, xi-1, yi) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi-1, yi, WATER);

                else
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, WATER);

                break;

              case SAND:
                if (!Legal(xi, yi+1)) continue; // OOB check
                                                               
                Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);

                if (Renderer::GetPixelAt(Renderer::newPixels, xi, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi+1, SAND);

                else if (Renderer::GetPixelAt(Renderer::newPixels, xi-1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi-1, yi+1, SAND);

                else if (Renderer::GetPixelAt(Renderer::newPixels, xi+1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi+1, yi+1, SAND);

                else
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, SAND);

                break;
            }
        }
    }
}
