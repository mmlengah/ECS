#pragma once
#include <SDL.h>
#include <memory>
#include "QuitManager.h"
#include "Camera.h"


class Game {
public:
    Game();

    ~Game();

    bool Initialize(const char* windowTitle, int screenWidth, int screenHeight);

    void Run();
private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> win;
    QuitManager& quitManager;

    std::shared_ptr<Camera> cam;
};


