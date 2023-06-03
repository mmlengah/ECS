#pragma once
#include <SDL.h>

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


};


