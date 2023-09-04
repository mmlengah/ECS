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
    std::unique_ptr<SystemManager> systemManager;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<WorldSpaceSystem> worldSpaceSystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<PhysicsSystem> physicsSystem;
    std::shared_ptr<ScriptSystem> scriptSystem;
    std::shared_ptr<Camera> cam;
    std::shared_ptr<Entity> player;
};


