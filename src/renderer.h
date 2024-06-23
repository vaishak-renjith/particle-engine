#pragma once

#include <SDL2/SDL.h>
#include "definitions.h"

namespace Renderer {
    extern SDL_Window* window;
    extern SDL_Renderer* renderer;

    extern int ogPixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    extern int newPixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    extern SDL_Texture* buffer;


    bool Init();
    void Close();

    int IdxToCoord(int idx);

    // these should really be size_t
    int GetPixelAt(int* arr, int xi, int yi);

    void SetPixelAt(int* arr, int xi, int yi, int color);

    void Render();
}

// Particle macros
#define VOID (int)0x000000FF
#define SAND (int)0x00FFFFFF
#define WATER (int)0xFF0000FF
