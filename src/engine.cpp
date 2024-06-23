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


    for (int xi = 0; xi < GRID_WIDTH; xi++) {
        for (int yi = 0; yi < GRID_HEIGHT; yi++) {
            switch (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi)) {
              case WATER:
                if (yi + 1 >= GRID_HEIGHT) continue; // OOB check
                                                               
                Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);

                if (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi+1, WATER);

                else if (Renderer::GetPixelAt(Renderer::ogPixels, xi-1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi-1, yi+1, WATER);

                else if (Renderer::GetPixelAt(Renderer::ogPixels, xi+1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi+1, yi+1, WATER);

                else if (Renderer::GetPixelAt(Renderer::ogPixels, xi+1, yi) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi+1, yi, WATER);

                else if (Renderer::GetPixelAt(Renderer::ogPixels, xi-1, yi) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi-1, yi, WATER);

                else
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, WATER);

                break;

              case SAND:
                if (yi + 1 >= GRID_HEIGHT) continue; // OOB check
                                                               
                Renderer::SetPixelAt(Renderer::newPixels, xi, yi, VOID);

                if (Renderer::GetPixelAt(Renderer::ogPixels, xi, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi+1, SAND);

                else if (Renderer::GetPixelAt(Renderer::ogPixels, xi-1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi-1, yi+1, SAND);

                else if (Renderer::GetPixelAt(Renderer::ogPixels, xi+1, yi+1) == VOID)
                    Renderer::SetPixelAt(Renderer::newPixels, xi+1, yi+1, SAND);

                else
                    Renderer::SetPixelAt(Renderer::newPixels, xi, yi, SAND);

                break;
            }
        }
    }
}
