#pragma once
#include <SDL.h>
#include <memory>
#include "Camera.h"
#include "ECS.h"

class Game {
public:
    Game();

    ~Game();

    bool Initialize(const char* windowTitle, int screenWidth, int screenHeight);

    void Run();
private:
    void Update();

    void Render();
private:
    bool quit;
    SDL_Window* window;
    SDL_Renderer* renderer;

    std::unique_ptr<Camera> camera;
    std::unique_ptr<RenderingSystem> renderingSystem;
    

};


