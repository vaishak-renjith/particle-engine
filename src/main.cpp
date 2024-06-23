#include <cmath>
#include <cstddef>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>

#include "particles.h"

// CONSTANTS
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int PIXEL_SIZE = 5;

const int GRID_WIDTH = SCREEN_WIDTH / PIXEL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / PIXEL_SIZE;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_bool app_quit;

int ogPixels[SCREEN_WIDTH * SCREEN_HEIGHT];
int newPixels[SCREEN_WIDTH * SCREEN_HEIGHT];

SDL_Texture* buffer = nullptr;


// FUNCTIONS
int idxToCoord(int idx) {
    return idx * PIXEL_SIZE;
}

int getPixelAt(int* arr, int xi, int yi) {
    int x = idxToCoord(xi);
    int y = idxToCoord(yi);

    size_t pos = x + y*SCREEN_WIDTH;
    return arr[pos];
}

void setPixelAt(int* arr, int xi, int yi, int color) {
    int x = idxToCoord(xi);
    int y = idxToCoord(yi);

    for (int xit = 0; xit < PIXEL_SIZE; xit++) {
        for (int yit = 0; yit < PIXEL_SIZE; yit++) {
            arr[x + y*SCREEN_WIDTH + xit + yit*SCREEN_WIDTH] = color;
        }
    }
}

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
    for (int xi = 0; xi < GRID_WIDTH; xi++) {
        for (int yi = 0; yi < GRID_HEIGHT; yi++) {
            setPixelAt(ogPixels, xi, yi, VOID);
            setPixelAt(newPixels, xi, yi, VOID);
        }
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

    const std::chrono::duration<double> delay{0.01};
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

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (spawnSand) {
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

            // std::cout << "pixel set at " << x << " " << y << std::endl;
            setPixelAt(ogPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, SAND);
            setPixelAt(newPixels, mouseX/PIXEL_SIZE, mouseY/PIXEL_SIZE, SAND);
        }

        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = now - last_update;

        if (diff < delay) continue;
        last_update = now;

        for (int xi = 0; xi < GRID_WIDTH; xi++) {
            for (int yi = 0; yi < GRID_HEIGHT; yi++) {
                switch (getPixelAt(ogPixels, xi, yi)) {
                  case SAND: // sand
                    if (yi + 1 >= GRID_HEIGHT) continue; // OOB check
                                                                   
                    setPixelAt(newPixels, xi, yi, VOID);

                    if (getPixelAt(ogPixels, xi, yi+1) == VOID)
                        setPixelAt(newPixels, xi, yi+1, SAND);

                    else if (getPixelAt(ogPixels, xi-1, yi+1) == VOID)
                        setPixelAt(newPixels, xi-1, yi+1, SAND);

                    else if (getPixelAt(ogPixels, xi+1, yi+1) == VOID)
                        setPixelAt(newPixels, xi+1, yi+1, SAND);

                    else
                        setPixelAt(newPixels, xi, yi, SAND);

                    break;
                }
            }
        }

        int pitch = SCREEN_WIDTH * sizeof(int); // pitch in bytes

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
