#include "gui.h"
#include <SDL2/SDL.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

// Define static member variables
SDL_Window* GUI::window = nullptr;
SDL_Renderer* GUI::renderer = nullptr;
SDL_Texture* GUI::drawTexture = nullptr;

// Initialize function implementation
bool GUI::Init(SDL_Window* w, SDL_Renderer* r) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Initialize ImGui for SDL2
    if (!ImGui_ImplSDL2_InitForSDLRenderer(w, r)) {
        std::cerr << "Failed to initialize ImGui for SDL2." << std::endl;
        return false;
    }
    
    // Initialize ImGuiRenderer2
    ImGui_ImplSDLRenderer2_Init(r);

    // Store window and renderer pointers
    GUI::window = w;
    GUI::renderer = r;

    return true;
}


// Close function implementation
void GUI::Close() {
    // Shutdown ImGui and destroy the context
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui::DestroyContext();
}

// Display function implementation
void GUI::Display() {
    // Start new frames for ImGui and ImGuiRenderer2
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // ImGui content setup
    ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(SDL_GetWindowSurface(window)->w - 210, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Tools");

    // ImGui buttons
    if (ImGui::Button("Sand")) {
        std::cout << "SAND" << std::endl;
    }
    if (ImGui::Button("Water")) {
        std::cout << "WATER" << std::endl;
    }
    if (ImGui::Button("Stone")) {
        std::cout << "STONE" << std::endl;
    }
    if (ImGui::Button("Acid")) {
        std::cout << "ACID" << std::endl;
    }

    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), GUI::renderer);

    // Render to SDL renderer
    SDL_RenderClear(GUI::renderer);
    SDL_RenderPresent(GUI::renderer);
}
