#include "definitions.h"
#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <chrono>
#include <iostream>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

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

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForSDLRenderer(Renderer::window, Renderer::renderer);
    ImGui_ImplSDLRenderer2_Init(Renderer::renderer);

    SDL_Log("Application started successfully!");

    bool quit = false;
    SDL_Event e;

    const std::chrono::duration<double> delay{0.01};
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

            
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> diff = now - last_update;

        if (diff < delay) continue;
        last_update = now;

        Engine::Loop();
        

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("debug");
        
        if (ImGui::Button("Sand")) {
            std::cout << "sand";
            Engine::currentParticle = SAND;
        }

        if (ImGui::Button("Water")) {
            std::cout << "water";
            Engine::currentParticle = WATER;
        }

        if (ImGui::Button("Stone")) {
            std::cout << "stone";
            Engine::currentParticle = STONE;
        }

        if (ImGui::Button("Acid")) {
            std::cout << "acid";
            Engine::currentParticle = ACID;
        }

        ImGui::End();
        
        ImGui::Render();

        Renderer::Render();

        //SDL_RenderClear(Renderer::renderer);

        // Render ImGui
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Renderer::renderer);

        SDL_RenderPresent(Renderer::renderer);
    }

    close();
    return 0;
}
