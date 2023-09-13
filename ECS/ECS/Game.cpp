#include "Game.h"
#include <SDL_image.h>
#include <iostream>

#include "Scene.h"
#include "LevelOne.h"


Game::Game() : win(nullptr, SDL_DestroyWindow), quitManager(QuitManager::getInstance())
{

}

Game::~Game()
{
    SDL_Quit();
}

bool Game::Initialize(const char* windowTitle, int screenWidth, int screenHeight)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    const int viewPortWidth = 640;
    const int viewPortHeight = 480;

    win.reset(SDL_CreateWindow("Hello World!", 100, 100, viewPortWidth, viewPortHeight, SDL_WINDOW_SHOWN));
    Renderer::Instance().Initialize(win.get());

    //add camera
    cam = std::make_shared<Camera>(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f), 0.0f, Vector2int(viewPortWidth, viewPortHeight));

    //load scenes here
    SceneManager::AddScene(std::make_shared<LevelOne>("LevelOne", cam));
    SceneManager::AddScene(std::make_shared<LevelTwo>("LevelTwo", cam));

    SceneManager::SwitchScene("LevelOne");

    return true;
}

void Game::Run()
{
    while (!quitManager.isQuit())
    {
        SceneManager::Run();
    }    
}
