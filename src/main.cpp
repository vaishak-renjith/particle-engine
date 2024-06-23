#include <SDL2/SDL_timer.h>
#include <cmath>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_main.h>
#include <iostream>
#include <chrono>

// 2d array (old, updated)
// logic
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_bool app_quit;

int ogPixels[SCREEN_WIDTH * SCREEN_HEIGHT];
int newPixels[SCREEN_WIDTH * SCREEN_HEIGHT];

SDL_Texture* buffer = nullptr;

bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return false;
    }

    window = SDL_CreateWindow("Window", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        return false;
    }
    
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!buffer) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }

    return true;
}

void close() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
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
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = now - last_update;

        if (diff < delay) continue;
        last_update = now;

        int b, g, r, a;
        b = rand()%255 << 24;
        g = rand()%255 << 16;
        r = rand()%255 << 8;
        a = rand()%255;

        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            ogPixels[i] = b|g|r|a;
        }

        int pitch = SCREEN_WIDTH * sizeof(int); // pitch in bytes

        int* pixels;
        if (SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch) == 0) {
            memcpy(pixels, ogPixels, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int));
            SDL_UnlockTexture(buffer);
        }

        SDL_RenderClear(renderer); // Clear the renderer with the current drawing color
        SDL_RenderCopy(renderer, buffer, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    close();
    return 0;
}
