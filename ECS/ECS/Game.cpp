#include "Game.h"
#include <SDL_image.h>
#include "ECS.h"
#include "Player.h"

Game::Game() : quit(false), oldTime(0), currentTime(0), deltaTime(0), win(nullptr, SDL_DestroyWindow),
renderer(nullptr, SDL_DestroyRenderer), renderSystem(nullptr), inputSystem(nullptr),
updateSystem(nullptr), worldSpaceSystem(nullptr), collisionSystem(nullptr), cam(nullptr), player(nullptr)
{

}

Game::~Game()
{
    delete renderSystem;
    delete inputSystem;
    delete updateSystem;
    delete worldSpaceSystem;
    delete collisionSystem;
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

    renderSystem = &(systemManager->registerSystem<RenderSystem>(renderer.get()));
    inputSystem = &(systemManager->registerSystem<InputSystem>());
    updateSystem = &(systemManager->registerSystem<UpdateSystem>());
    worldSpaceSystem = &(systemManager->registerSystem<WorldSpaceSystem>(cam));
    collisionSystem = &(systemManager->registerSystem<CollisionSystem>());
    
    player = createPlayerPrefab(renderer.get());

    Rectangle a = { 10, 10 };
    SDL_Color white = { 255, 255, 255, 255 };

    auto box = Entity::create();
    box->addComponent<TransformComponent>(Vector2f(0, 0), 0.0f, Vector2f(2.0f, 2.0f));
    box->addComponent<SquareComponent>(a, white);
    box->addComponent<BoxColliderComponent>();

    auto box2 = Entity::create();
    box2->addComponent<TransformComponent>(Vector2f(620, 0), 0.0f, Vector2f(3.0f, 3.0f));
    box2->addComponent<SquareComponent>(a, white);
    box2->addComponent<BoxColliderComponent>();

    auto box3 = Entity::create();
    box3->addComponent<TransformComponent>(Vector2f(0, 460), 0.0f, Vector2f(2.0f, 2.0f));
    box3->addComponent<SquareComponent>(a, white);
    box3->addComponent<BoxColliderComponent>();

    auto box4 = Entity::create();
    box4->addComponent<TransformComponent>(Vector2f(620, 460), 0.0f, Vector2f(20.0f, 20.0f));
    box4->addComponent<SquareComponent>(a, white);    
    box4->addComponent<BoxColliderComponent>();

    systemManager->addAllEntitiesToSystems(Entity::getAllEntities());

    return true;
}

void Game::Run()
{
    // Event handler
    SDL_Event event;

    // Game loop
    while (!quit) {
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - oldTime) / 1000.0f; // convert from milliseconds to seconds
        oldTime = currentTime;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            inputSystem->update(event);
        }

        // Update game logic
        Update();

        // Render the game
        Render();
    }
}

void Game::Update()
{
    cam->lookAt(player->getComponent<TransformComponent>()->getPosition());
    updateSystem->update(deltaTime);
    collisionSystem->update();
    worldSpaceSystem->update();
}

void Game::Render()
{
    renderSystem->update();
}
