#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#include "renderer.h"
#include "engine.h"


// CONSTANTS
SDL_bool app_quit;


// FUNCTIONS
bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return false;
    }

    return (Renderer::Init());
}

void close() {
    Renderer::Close();
}

int main() {
    if (!init()) {
        return -1;
    }

    SDL_Log("Application started successfully!");

    bool quit = false;
    SDL_Event e;

    const std::chrono::duration<double> delay{1};
    auto last_update = std::chrono::steady_clock::now();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case (SDL_QUIT):
                  quit = true;
                  break;
                case SDL_MOUSEBUTTONDOWN:
                  Engine::spawnParticles = true;
                  break;
                case SDL_MOUSEBUTTONUP:
                  Engine::spawnParticles = false;
                  break;
                case SDL_KEYDOWN:
                  Engine::HandleKeypress(e.key);
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = now - last_update;

        if (diff < delay) continue;
        last_update = now;

        Engine::Loop();
        Renderer::Render();
    }

    close();
    return 0;
}
