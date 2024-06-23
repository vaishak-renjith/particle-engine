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

    int GetPixelAt(int* arr, int xi, int yi);

    void SetPixelAt(int* arr, int xi, int yi, int color);

    void Render();
}
