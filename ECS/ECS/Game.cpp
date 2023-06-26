#include "Game.h"
#include <SDL_image.h>
#include "ECS.h"

Game::Game() : quit(false), win(nullptr, SDL_DestroyWindow), renderer(nullptr, SDL_DestroyRenderer), renderSystem(nullptr)
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

    win.reset(SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN));
    renderer.reset(SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    systemManager = std::make_unique<SystemManager>();

    renderSystem = &(systemManager->registerSystem<RenderSystem>(renderer.get()));

    auto player = Entity::create();
    player->addComponent<TransformComponent>(Vector2f(320, 240), 0.0f, Vector2f(2.0f, 2.0f));
    SDL_Rect a = { 0, 0, 10, 10 };
    player->addComponent<SpriteComponent>(a);

    systemManager->addAllEntitiesToSystems(Entity::getAllEntities());

    //add camera

    return true;
}

void Game::Run()
{
    // Event handler
    SDL_Event event;

    // Game loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Update game logic
        Update();

        // Render the game
        Render();
    }
}

void Game::Update()
{
}

void Game::Render()
{
    renderSystem->update();
}
