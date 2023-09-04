#include "Game.h"
#include <SDL_image.h>
#include "ECS.h"
#include "Player.h"
#include <iostream>
#include "Renderer.h"

class BoxMovementScript : public Script {
public:
    BoxMovementScript() : speed(1000.0f), transform(nullptr), physics(nullptr) {}

    void start() override {
        transform = entity.lock()->getComponent<TransformComponent>();
        physics = entity.lock()->getComponent<PhysicsComponent>();
    }

    void update(float deltaTime) override {
        physics->applyForce(Vector2f(speed, 0.0f) * deltaTime);
    }

    void onCollision(const std::shared_ptr<Entity>& other) override {
        speed = 0;
        physics->isAffectedByGravity = true;
    }

    void setSpeed(float speed) {
        this->speed = speed;
    }

    float getSpeed() const {
        return speed;
    }
private:
    float speed;
    std::shared_ptr<TransformComponent> transform;
    std::shared_ptr<PhysicsComponent> physics;
};

Game::Game() : quit(false), oldTime(0), currentTime(0), deltaTime(0), win(nullptr, SDL_DestroyWindow),
renderSystem(nullptr), worldSpaceSystem(nullptr),
collisionSystem(nullptr), physicsSystem(nullptr), scriptSystem(nullptr), cam(nullptr), player(nullptr)
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

    systemManager = std::make_unique<SystemManager>();

    renderSystem = systemManager->registerSystem<RenderSystem>(true);
    worldSpaceSystem = systemManager->registerSystem<WorldSpaceSystem>(cam);
    collisionSystem = systemManager->registerSystem<CollisionSystem>();
    physicsSystem = systemManager->registerSystem<PhysicsSystem>();
    scriptSystem = systemManager->registerSystem<ScriptSystem>();

    collisionSystem->collisionMatrix.setShouldCollide(LayerOne, LayerTwo, false);
    
    player = createPlayerPrefab();

    Rectangle a = { 10, 10 };
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color orange = { 255, 165, 0, 255 };

    auto box = Entity::create();
    box->addComponent<TransformComponent>(Vector2f(0, 0), 0.0f, Vector2f(2.0f, 2.0f));
    box->addComponent<SquareComponent>(a, white);
    box->addComponent<BoxColliderComponent>();
    box->addComponent<PhysicsComponent>(10.0f, false, true);

    auto box2 = Entity::create();
    box2->name = "orange";
    box2->addComponent<TransformComponent>(Vector2f(620, 0), 0.0f, Vector2f(3.0f, 3.0f));
    box2->addComponent<SquareComponent>(a, orange);
    box2->addComponent<BoxColliderComponent>();
    box2->addComponent<PhysicsComponent>(10.f, true, false);

    box2->getComponent<BoxColliderComponent>()->setLayer(LayerTwo);

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
    box5->addComponent<PhysicsComponent>(40.f, false);
    box5->addComponent<ScriptComponent>();
    auto box5physics = box5->getComponent<PhysicsComponent>();
    auto box5BoxCollider = box5->getComponent<BoxColliderComponent>();
    auto box5ScriptComponent = box5->getComponent<ScriptComponent>();

    box5ScriptComponent->addScript(std::make_shared<BoxMovementScript>());

    SDL_Color red = { 255, 0, 0, 255 };
    auto box6 = Entity::create();
    box6->addComponent<TransformComponent>(Vector2f(520, 40), 45.0f, Vector2f(2.0f, 2.0f));
    box6->addComponent<SquareComponent>(a, red);
    box6->addComponent<BoxColliderComponent>();
    box6->addComponent<PhysicsComponent>(10.f, false);
    box6->addComponent<ScriptComponent>();

    auto box6physics = box6->getComponent<PhysicsComponent>();
    auto box6BoxCollider = box6->getComponent<BoxColliderComponent>();
    auto box6ScriptComponent = box6->getComponent<ScriptComponent>();

    std::shared_ptr<BoxMovementScript> boxMovementScript = std::make_shared<BoxMovementScript>();
    boxMovementScript->setSpeed(-1000.0f);
    
    box6ScriptComponent->addScript(boxMovementScript);

    systemManager->addAllEntitiesToSystems(Entity::getAllEntities());

    return true;
}

void Game::Run()
{
    // Event handler
    SDL_Event event;

    // Game loop
    scriptSystem->start();

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
    cam->lookAt(player->getComponent<TransformComponent>()->getPosition());
    worldSpaceSystem->update();
    scriptSystem->update(deltaTime);
    collisionSystem->update();
    physicsSystem->update(deltaTime);      
}

void Game::Render()
{
    renderSystem->update(deltaTime);
}
