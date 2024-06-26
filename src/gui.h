#pragma once

#include <SDL2/SDL.h>
#include <iostream>

#include "renderer.h"
#include "engine.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace GUI {
    extern SDL_Window* window;
    extern SDL_Renderer* renderer;
    extern SDL_Texture* drawTexture;

    bool Init(SDL_Window* w, SDL_Renderer* r);
    void Close();
    void Display();
}
