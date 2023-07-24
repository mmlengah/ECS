#include "Game.h"
#include <SDL_image.h>
#include "ECS.h"
#include "Player.h"
#include <iostream>
#include "Renderer.h"


Game::Game() : quit(false), oldTime(0), currentTime(0), deltaTime(0), win(nullptr, SDL_DestroyWindow),
renderSystem(nullptr), inputSystem(nullptr), updateSystem(nullptr), worldSpaceSystem(nullptr),
collisionSystem(nullptr), physicsSystem(nullptr), cam(nullptr), player(nullptr)
{

}

Game::~Game()
{
    delete renderSystem;
    delete inputSystem;
    delete updateSystem;
    delete worldSpaceSystem;
    delete collisionSystem;
    delete physicsSystem;
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

    //add camera
    cam = new Camera(Vector2f(0, 0), Vector2f(1, 1), 0, Vector2int(viewPortWidth, viewPortHeight));

    systemManager = std::make_unique<SystemManager>();

    renderSystem = &(systemManager->registerSystem<RenderSystem>());
    inputSystem = &(systemManager->registerSystem<InputSystem>());
    updateSystem = &(systemManager->registerSystem<UpdateSystem>());
    worldSpaceSystem = &(systemManager->registerSystem<WorldSpaceSystem>(cam));
    collisionSystem = &(systemManager->registerSystem<CollisionSystem>());
    physicsSystem = &(systemManager->registerSystem<PhysicsSystem>());
    
    player = createPlayerPrefab();

    Rectangle a = { 10, 10 };
    SDL_Color white = { 255, 255, 255, 255 };

    auto box = Entity::create();
    box->addComponent<TransformComponent>(Vector2f(0, 0), 0.0f, Vector2f(2.0f, 2.0f));
    box->addComponent<SquareComponent>(a, white);
    box->addComponent<BoxColliderComponent>();
    box->addComponent<PhysicsComponent>(10.0f, false, true);

    auto box2 = Entity::create();
    box2->addComponent<TransformComponent>(Vector2f(620, 0), 0.0f, Vector2f(3.0f, 3.0f));
    box2->addComponent<SquareComponent>(a, white);
    box2->addComponent<BoxColliderComponent>();
    box2->addComponent<PhysicsComponent>(10.f, false, true);

    auto box3 = Entity::create();
    box3->addComponent<TransformComponent>(Vector2f(0, 460), 0.0f, Vector2f(2.0f, 2.0f));
    box3->addComponent<SquareComponent>(a, white);
    box3->addComponent<BoxColliderComponent>();
    box3->addComponent<PhysicsComponent>(10.f, false, true);

    auto box4 = Entity::create();
    box4->addComponent<TransformComponent>(Vector2f(620, 460), 0.0f, Vector2f(20.0f, 20.0f));
    box4->addComponent<SquareComponent>(a, white);
    box4->addComponent<BoxColliderComponent>();
    box4->addComponent<PhysicsComponent>(10.f, false, true);

    SDL_Color blue = { 0, 0, 255, 255 };
    auto box5 = Entity::create();
    box5->addComponent<TransformComponent>(Vector2f(120, 40), 0.0f, Vector2f(4.0f, 4.0f));
    box5->addComponent<SquareComponent>(a, blue);
    box5->addComponent<BoxColliderComponent>();
    box5->addComponent<UpdateComponent>();
    box5->addComponent<PhysicsComponent>(40.f, false);
    UpdateComponent* box5update = box5->getComponent<UpdateComponent>();
    PhysicsComponent* box5physics = box5->getComponent<PhysicsComponent>();
    BoxColliderComponent* box5BoxCollider = box5->getComponent<BoxColliderComponent>();

    auto speed = std::make_shared<float>(1000.0f);

    box5update->addUpdateFunction([speed, box5physics](Entity& entity, float deltaTime) {
        if (*speed > 0) {
            box5physics->applyForce(Vector2f(*speed, 0.0f) * deltaTime);
        }
        });

    box5BoxCollider->addCollisionHandler([speed, box5physics](Entity* self, Entity* other) {
        *speed = 0;
        box5physics->isAffectedByGravity = true;
        });

    SDL_Color red = { 255, 0, 0, 255 };
    auto box6 = Entity::create();
    box6->addComponent<TransformComponent>(Vector2f(520, 40), 45.0f, Vector2f(2.0f, 2.0f));
    box6->addComponent<SquareComponent>(a, red);
    box6->addComponent<BoxColliderComponent>();
    box6->addComponent<UpdateComponent>();
    box6->addComponent<PhysicsComponent>(10.f, false);
    UpdateComponent* box6update = box6->getComponent<UpdateComponent>();
    PhysicsComponent* box6physics = box6->getComponent<PhysicsComponent>();
    BoxColliderComponent* box6BoxCollider = box6->getComponent<BoxColliderComponent>();

    box6update->addUpdateFunction([speed, box6physics](Entity& entity, float deltaTime) {        
        if (*speed > 0) {
            box6physics->applyForce(Vector2f(-(*speed), 0.0f) * deltaTime);
        }
        
    });

    box6BoxCollider->addCollisionHandler([speed, box6physics](Entity* self, Entity* other) {
        *speed = 0;
        box6physics->isAffectedByGravity = true;
    });

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
    worldSpaceSystem->update();
    updateSystem->update(deltaTime);
    collisionSystem->update();
    physicsSystem->update(deltaTime);      
}

void Game::Render()
{
    renderSystem->update(deltaTime);
}
