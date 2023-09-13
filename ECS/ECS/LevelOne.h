#pragma once
#include "Scene.h"
#include "Player.h"

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

class LevelOne : public Scene {
public:
    LevelOne(const std::string& name, std::shared_ptr<Camera> cam) : Scene(name, cam) {}

    void Load() override {
        Rectangle a = { 10, 10 };
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Color orange = { 255, 165, 0, 255 };

        player = createPlayerPrefab();
        SetCameraTarget(player);

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
        boxMovementScript->setSpeed(-1500.0f);

        box6ScriptComponent->addScript(boxMovementScript);
    }
private:
    std::shared_ptr<Entity> player;
};

class LevelTwo : public Scene {
public:
    LevelTwo(const std::string& name, std::shared_ptr<Camera> cam) : Scene(name, cam) {}

    void Load() override {
        setShouldCollide(LayerOne, LayerTwo, false);

        player = createPlayerPrefab();
        SetCameraTarget(player);

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
        boxMovementScript->setSpeed(-1500.0f);

        box6ScriptComponent->addScript(boxMovementScript);
    }

private:
    std::shared_ptr<Entity> player;
};