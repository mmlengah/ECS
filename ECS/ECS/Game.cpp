#include "Game.h"
#include <SDL_image.h>
#include "ECS.h"

Game::Game() : quit(false), win(nullptr, SDL_DestroyWindow), renderer(nullptr, SDL_DestroyRenderer),
renderSystem(nullptr), cam(nullptr), player(nullptr)
{
    
}

Game::~Game()
{
    delete renderSystem;
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
    renderer.reset(SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    //add camera
    cam = new Camera(Vector2f(0, 0), Vector2f(1, 1), 0, Vector2int(viewPortWidth, viewPortHeight));

    systemManager = std::make_unique<SystemManager>();

    renderSystem = &(systemManager->registerSystem<RenderSystem>(renderer.get(), cam));

    player = Entity::create();
    player->addComponent<TransformComponent>(Vector2f(320, 240), 0.0f, Vector2f(2.0f, 2.0f));
    SDL_Rect a = { 0, 0, 10, 10 };
    player->addComponent<SpriteComponent>(a);

    auto box = Entity::create();
    box->addComponent<TransformComponent>(Vector2f(0, 0), 0.0f, Vector2f(2.0f, 2.0f));
    box->addComponent<SpriteComponent>(a);

    auto box2 = Entity::create();
    box2->addComponent<TransformComponent>(Vector2f(620, 0), 0.0f, Vector2f(2.0f, 2.0f));
    box2->addComponent<SpriteComponent>(a);

    auto box3 = Entity::create();
    box3->addComponent<TransformComponent>(Vector2f(0, 460), 0.0f, Vector2f(2.0f, 2.0f));
    box3->addComponent<SpriteComponent>(a);

    auto box4 = Entity::create();
    box4->addComponent<TransformComponent>(Vector2f(620, 460), 0.0f, Vector2f(2.0f, 2.0f));
    box4->addComponent<SpriteComponent>(a);

    systemManager->addAllEntitiesToSystems(Entity::getAllEntities());

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
