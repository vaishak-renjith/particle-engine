#include <cmath>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>

// 2d array (old, updated)
// logic
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int PIXEL_SIZE = 10;

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

    // fill texture to be black
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        ogPixels[i] = 0x000000FF;
        newPixels[i] = 0x000000FF;
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

    const std::chrono::duration<double> delay{0.5};
    auto last_update = std::chrono::steady_clock::now();

    bool spawnSand = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case (SDL_QUIT):
                  quit = true;
                  break;
                case SDL_MOUSEBUTTONDOWN:
                  spawnSand = true;
                  break;
                case SDL_MOUSEBUTTONUP:
                  spawnSand = false;
                  break;
            }
        }

        int x, y;
        SDL_GetMouseState(&x, &y);

        if (spawnSand) {
            // snap x to closest on-grid point
            int leftDist = x%PIXEL_SIZE;

            if (leftDist < PIXEL_SIZE - leftDist) {
                x -= leftDist;
            } else {
                x += PIXEL_SIZE - leftDist;
            }

            // snap y to closest on-grid point
            int topDist = y - (y/PIXEL_SIZE*PIXEL_SIZE);

            if (topDist < PIXEL_SIZE - topDist) {
                y -= topDist;
            } else {
                y += PIXEL_SIZE - topDist;
            }

            int p = x + y*SCREEN_WIDTH;

            for (int pw = 0; pw < PIXEL_SIZE; pw++) {
                for (int ph = 0; ph < PIXEL_SIZE; ph++) {
                    newPixels[p + pw + (ph+1)*SCREEN_WIDTH] = 0x00FFFFFF;
                }
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = now - last_update;

        if (diff < delay) continue;
        last_update = now;

        int pitch = SCREEN_WIDTH * sizeof(int); // pitch in bytes


        for (int p = 0; p < SCREEN_WIDTH*SCREEN_HEIGHT; p++) {
            if (p % PIXEL_SIZE != 0) continue;
            if ((p / SCREEN_WIDTH) % PIXEL_SIZE != 0) continue;

            switch (ogPixels[p]) {
              case 0x00FFFFFF: // sand
                // check if empty below
                if (p+SCREEN_WIDTH >= SCREEN_WIDTH*SCREEN_HEIGHT) continue; // OOB check
                if (ogPixels[p+SCREEN_WIDTH] == 0x000000FF) { // check if empty below
                    // update new pixels
                    for (int pw = 0; pw < PIXEL_SIZE; pw++) {
                        for (int ph = 0; ph < PIXEL_SIZE; ph++) {
                        }
                    }
                }

                break;

              case 0x000000FF: // blank
                break;
            }
        }

        int* pixels;
        if (SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch) == 0) {
            memcpy(pixels, newPixels, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int));
            memcpy(ogPixels, newPixels, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int));
            SDL_UnlockTexture(buffer);
        }

        SDL_RenderClear(renderer); // Clear the renderer with the current drawing color
        SDL_RenderCopy(renderer, buffer, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    close();
    return 0;
}
