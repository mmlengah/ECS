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
    Uint32 oldTime, currentTime;
    float deltaTime;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> win;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    std::unique_ptr<SystemManager> systemManager;
    RenderSystem* renderSystem;
    InputSystem* inputSystem;        
    UpdateSystem* updateSystem;
    WorldSpaceSystem* worldSpaceSystem;
    CollisionSystem* collisionSystem;
    Camera* cam;
    std::shared_ptr<Entity> player;
};


