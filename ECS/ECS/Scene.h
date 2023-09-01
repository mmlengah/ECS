#pragma once
#include <map>
#include <string>
#include "Camera.h"
#include "Player.h"
#include "ECS.h"

class Scene {
public:
    Scene(Camera* cam) {
        this->cam = std::make_shared<Camera>(*cam);

        systemManager = std::make_unique<SystemManager>();

        renderSystem = &(systemManager->registerSystem<RenderSystem>(true));
        worldSpaceSystem = &(systemManager->registerSystem<WorldSpaceSystem>(this->cam));
        collisionSystem = &(systemManager->registerSystem<CollisionSystem>());
        physicsSystem = &(systemManager->registerSystem<PhysicsSystem>());
        scriptSystem = &(systemManager->registerSystem<ScriptSystem>());

    }

    ~Scene() {
        delete renderSystem;
        delete worldSpaceSystem;
        delete collisionSystem;
        delete physicsSystem;
        delete scriptSystem;
    };

    virtual void init() {

    }

    void update(float deltaTime) {
        setCameraLookAt(getUpdateFocusPoint());
        worldSpaceSystem->update();
        scriptSystem->update(deltaTime);
        collisionSystem->update();
        physicsSystem->update(deltaTime);
    };

    void render(float deltaTime) {
        renderSystem->update(deltaTime);
    }

    void setShouldCollide(CollisionLayer layer1, CollisionLayer layer2, bool shouldCollide) {
        collisionSystem->collisionMatrix.setShouldCollide(layer1, layer2, shouldCollide);
    }

    void registerAllEntities() {
        systemManager->addAllEntitiesToSystems(Entity::getAllEntities());
    }

    void startScripts() {
        scriptSystem->start();
    }

    virtual Vector2f getUpdateFocusPoint() {       
        return cam->getPosition();
    }
private:
    void setCameraLookAt(const Vector2f& position) {
        cam->lookAt(position);
    }
private:
    std::shared_ptr<Camera> cam;
    std::unique_ptr<SystemManager> systemManager;

    RenderSystem* renderSystem;
    WorldSpaceSystem* worldSpaceSystem;
    CollisionSystem* collisionSystem;
    PhysicsSystem* physicsSystem;
    ScriptSystem* scriptSystem;


};

class SceneManager {
public:
    // Singleton pattern to make sure we have one instance of SceneManager.
    static SceneManager& getInstance() {
        static SceneManager instance;
        return instance;
    }

    // Prevent multiple instantiations
    SceneManager(SceneManager const&) = delete;
    void operator=(SceneManager const&) = delete;

    void SwitchScene(const std::string& sceneName) {
        // Unload the current scene
        if (currentScene) {
            //currentScene->Unload();
        }

        currentScene = scenes[sceneName];
        currentScene->init();
        currentScene->registerAllEntities();
        currentScene->startScripts();
    }

    void update(float deltaTime) {
        if (currentScene) {
            currentScene->update(deltaTime);
        }
    }

    void render(float deltaTime) {
        if (currentScene) {
            currentScene->render(deltaTime);
        }
    }

    void RegisterScene(const std::string& sceneName, std::shared_ptr<Scene> scene) {
        scenes[sceneName] = scene;
    }

private:
    SceneManager() = default;
    std::map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> currentScene = nullptr;
};

class BoxMovementScript : public Script {
public:
    BoxMovementScript() : speed(1000.0f), transform(nullptr), physics(nullptr) {}

    void start() override {
        transform = entity->getComponent<TransformComponent>();
        physics = entity->getComponent<PhysicsComponent>();
    }

    void update(float deltaTime) override {
        physics->applyForce(Vector2f(speed, 0.0f) * deltaTime);
    }

    void onCollision(Entity* other) override {
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
    TransformComponent* transform;
    PhysicsComponent* physics;
};

class LevelOneScene : public Scene {
public:
    LevelOneScene(Camera* cam) : Scene(cam) {
       
    }

    void init() override {
        setShouldCollide(Default, Default, false);

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
        box4->getComponent<BoxColliderComponent>()->setLayer(LayerOne);

        SDL_Color blue = { 0, 0, 255, 255 };
        auto box5 = Entity::create();
        box5->addComponent<TransformComponent>(Vector2f(120, 40), 0.0f, Vector2f(4.0f, 4.0f));
        box5->addComponent<SquareComponent>(a, blue);
        box5->addComponent<BoxColliderComponent>();
        box5->addComponent<PhysicsComponent>(40.f, false);
        box5->addComponent<ScriptComponent>();
        PhysicsComponent* box5physics = box5->getComponent<PhysicsComponent>();
        BoxColliderComponent* box5BoxCollider = box5->getComponent<BoxColliderComponent>();
        ScriptComponent* box5ScriptComponent = box5->getComponent<ScriptComponent>();

        box5ScriptComponent->addScript(std::make_shared<BoxMovementScript>());

        SDL_Color red = { 255, 0, 0, 255 };
        auto box6 = Entity::create();
        box6->addComponent<TransformComponent>(Vector2f(520, 40), 45.0f, Vector2f(2.0f, 2.0f));
        box6->addComponent<SquareComponent>(a, red);
        box6->addComponent<BoxColliderComponent>();
        box6->addComponent<PhysicsComponent>(10.f, false);
        box6->addComponent<ScriptComponent>();

        PhysicsComponent* box6physics = box6->getComponent<PhysicsComponent>();
        BoxColliderComponent* box6BoxCollider = box6->getComponent<BoxColliderComponent>();
        ScriptComponent* box6ScriptComponent = box6->getComponent<ScriptComponent>();

        std::shared_ptr<BoxMovementScript> boxMovementScript = std::make_shared<BoxMovementScript>();
        boxMovementScript->setSpeed(-1000.0f);

        box6ScriptComponent->addScript(boxMovementScript);
    }

    virtual Vector2f getUpdateFocusPoint() override {
        return player->getComponent<TransformComponent>()->getPosition();
    }

private:
    std::shared_ptr<Entity> player;
};

