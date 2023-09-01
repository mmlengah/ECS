#include "Game.h"
#include <SDL_image.h>
#include "ECS.h"
#include <iostream>
#include "Renderer.h"

Game::Game() : quit(false), oldTime(0), currentTime(0), deltaTime(0), win(nullptr, SDL_DestroyWindow), cam(nullptr), 
sceneManager(nullptr)
{

}

Game::~Game()
{
    delete sceneManager;
    delete cam;
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

    cam = new Camera(Vector2f(0, 0), Vector2f(1, 1), 0, Vector2int(viewPortWidth, viewPortHeight));

    auto levelOne = std::make_shared<LevelOneScene>(cam);

    sceneManager = &SceneManager::getInstance();

    sceneManager->RegisterScene("LevelOne", levelOne);

    sceneManager->SwitchScene("LevelOne");

    return true;
}

void Game::Run()
{
    // Event handler
    SDL_Event event;

    while (!quit) {
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - oldTime) / 1000.0f; // convert from milliseconds to seconds
        oldTime = currentTime;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            InputSystem::getInstance().update(event);
        }

        // Update game logic
        Update();

        // Render the game
        Render();
    }
}

void Game::Update()
{
    sceneManager->update(deltaTime);
}

void Game::Render()
{
    sceneManager->render(deltaTime);
}
