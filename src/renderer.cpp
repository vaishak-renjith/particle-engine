#include <SDL2/SDL.h>
#include "definitions.h"
#include "renderer.h"

SDL_Window* Renderer::window = nullptr;
SDL_Renderer* Renderer::renderer = nullptr;

int Renderer::ogPixels[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
int Renderer::newPixels[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};

SDL_Texture* Renderer::buffer = nullptr;

bool Renderer::Init() {
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
            SetPixelAt(ogPixels, xi, yi, VOID);
            SetPixelAt(newPixels, xi, yi, VOID);
        }
    }

    return true;
}

void Renderer::Close() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

int Renderer::IdxToCoord(int idx) {
    return idx * PIXEL_SIZE;
}

int Renderer::GetPixelAt(int* arr, int xi, int yi) {
    int x = IdxToCoord(xi);
    int y = IdxToCoord(yi);

    int pos = x + y*SCREEN_WIDTH;
    return arr[pos];
}

void Renderer::SetPixelAt(int* arr, int xi, int yi, int color) {
    int x = IdxToCoord(xi);
    int y = IdxToCoord(yi);

    for (int xit = 0; xit < PIXEL_SIZE; xit++) {
        for (int yit = 0; yit < PIXEL_SIZE; yit++) {
            arr[x + y*SCREEN_WIDTH + xit + yit*SCREEN_WIDTH] = color;
        }
    }
}

void Renderer::Render() {
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
