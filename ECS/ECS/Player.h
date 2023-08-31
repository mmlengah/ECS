#pragma once
#include <SDL.h>
#include "ECS.h"
#include "PCM.h"

using namespace PC;

class PlayerAnimationScript : public Script {
public:
    void start() override {
        velocity = entity->getComponent<VelocityComponent>();
        transform = entity->getComponent<TransformComponent>();
        sprite = entity->getComponent<SpriteComponent>();

        sprite->addAnimationState("idleFront", AnimationState(6 * 0, 6, 125));
        sprite->addAnimationState("idleRight", AnimationState(6 * 1, 6, 125));
        sprite->addAnimationState("idleLeft", AnimationState(6 * 1, 6, 125, SDL_FLIP_HORIZONTAL));
        sprite->addAnimationState("idleBack", AnimationState(6 * 2, 6, 125));
        sprite->addAnimationState("walkingFront", AnimationState(6 * 3, 6, 125));
        sprite->addAnimationState("walkingRight", AnimationState(6 * 4, 6, 125));
        sprite->addAnimationState("walkingLeft", AnimationState(6 * 4, 6, 125, SDL_FLIP_HORIZONTAL));
        sprite->addAnimationState("walkingBack", AnimationState(6 * 5, 6, 125));
        sprite->addAnimationState("attackingFront", AnimationState(6 * 6, 4, 125));
        sprite->addAnimationState("attackingRight", AnimationState(6 * 6 + 4, 4, 125));
        sprite->addAnimationState("attackingLeft", AnimationState(6 * 6 + 4, 4, 125, SDL_FLIP_HORIZONTAL));
        sprite->addAnimationState("attackingBack", AnimationState(6 * 6 + 4 * 2, 4, 125));
        sprite->addAnimationState("death", AnimationState(6 * 6 + 4 * 3, 4, 125));

        sprite->setAnimationState("idleFront");
    }

    void update(float deltaTime) override {
        if (velocity->dx > 0) {
            // Moving right
            sprite->setAnimationState(!sprite->animationPlaying && sprite->currentState == "attackingRight" ? "idleRight" : "walkingRight");
        }
        else if (velocity->dx < 0) {
            // Moving left
            sprite->setAnimationState(!sprite->animationPlaying && sprite->currentState == "attackingLeft" ? "idleLeft" : "walkingLeft");
        }
        else if (velocity->dy > 0) {
            // Moving down
            sprite->setAnimationState(!sprite->animationPlaying && sprite->currentState == "attackingFront" ? "idleFront" : "walkingFront");
        }
        else if (velocity->dy < 0) {
            // Moving up
            sprite->setAnimationState(!sprite->animationPlaying && sprite->currentState == "attackingBack" ? "idleBack" : "walkingBack");
        }
        else if (!sprite->animationPlaying) {
            if (sprite->currentState == "attackingRight" || sprite->currentState == "walkingRight") {
                sprite->setAnimationState("idleRight");
            }
            else if (sprite->currentState == "attackingLeft" || sprite->currentState == "walkingLeft") {
                sprite->setAnimationState("idleLeft");
            }
            else if (sprite->currentState == "attackingFront" || sprite->currentState == "walkingFront") {
                sprite->setAnimationState("idleFront");
            }
            else if (sprite->currentState == "attackingBack" || sprite->currentState == "walkingBack") {
                sprite->setAnimationState("idleBack");
            }
        }
    }

private:
    VelocityComponent* velocity;
    TransformComponent* transform;
    SpriteComponent* sprite;
};

class PlayerMovementScript : public Script {
public:
    void start() override {
        velocity = entity->getComponent<VelocityComponent>();
        transform = entity->getComponent<TransformComponent>();
    }

    void update(float deltaTime) override {
        transform->setPosition({ transform->getPosition().x + velocity->dx * deltaTime,
                                transform->getPosition().y + velocity->dy * deltaTime });

    }
private:
    VelocityComponent* velocity;
    TransformComponent* transform;
};

class PlayerInputScript : public Script {
public:
    void start() override {
        velocity = entity->getComponent<VelocityComponent>();
        transform = entity->getComponent<TransformComponent>();
        sprite = entity->getComponent<SpriteComponent>();
    }

    void update(float deltaTime) override {        
        if (InputSystem::isKeyUp(SDLK_w) || InputSystem::isKeyUp(SDLK_s)) {
            velocity->dy = 0;
        }
        if (InputSystem::isKeyUp(SDLK_a) || InputSystem::isKeyUp(SDLK_d)) {
            velocity->dx = 0;
        }
        if (InputSystem::isMouseButtonDown(SDL_BUTTON_LEFT)) {
            if (sprite->currentState == "walkingRight" || sprite->currentState == "idleRight") {
                sprite->setAnimationState("attackingRight");
            }
            else if (sprite->currentState == "walkingLeft" || sprite->currentState == "idleLeft") {
                sprite->setAnimationState("attackingLeft");
            }
            else if (sprite->currentState == "walkingFront" || sprite->currentState == "idleFront") {
                sprite->setAnimationState("attackingFront");
            }
            else if (sprite->currentState == "walkingBack" || sprite->currentState == "idleBack") {
                sprite->setAnimationState("attackingBack");
            }
            velocity->dx = 0;
            velocity->dy = 0;
        }
        if (InputSystem::isKeyDown(SDLK_w)) {
            if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
                && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
                velocity->dy = -velocity->dyMax;
            }
        }
        if (InputSystem::isKeyDown(SDLK_s)) {
            if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
                && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
                velocity->dy = velocity->dyMax;
            }
        }
        if (InputSystem::isKeyDown(SDLK_a)) {
            if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
                && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
                velocity->dx = -velocity->dxMax;
            }
        }
        if (InputSystem::isKeyDown(SDLK_d)) {
            if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
                && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
                velocity->dx = velocity->dxMax;
            }
        }
        

        
    }
private:
    VelocityComponent* velocity;
    TransformComponent* transform;
    SpriteComponent* sprite;
};

class PlayerCollisionScript : public Script {
public:
    void onCollisionExit(Entity* other) override {
        if (other->name == "orange") {
            std::cout << "collision with orange ended" << std::endl;
        }
    }

    void onCollision(Entity* other) override {
        if (other->name == "orange") {
            std::cout << "collision with orange" << std::endl;
        }
    }
};

std::shared_ptr<Entity> createPlayerPrefab() {
    // Create a new entity
    std::shared_ptr<Entity> player = Entity::create();

    // Add necessary components
    player->addComponent<TransformComponent>(Vector2f(320, 140), 0.0f, Vector2f(3.f, 3.f));
    player->addComponent<SpriteComponent>("Assets/mystic_woods_2.1/sprites/characters/player.png");
    player->addComponent<VelocityComponent>(100, 100);  
    player->addComponent<BoxColliderComponent>();
    player->addComponent<PhysicsComponent>(80.0f, false);
    player->addComponent<RenderLayerComponent>(1);
    player->addComponent<ScriptComponent>();

    // Get the InputComponent and bind keys to commands
    VelocityComponent* velocity = player->getComponent<VelocityComponent>();
    TransformComponent* transform = player->getComponent<TransformComponent>();
    SpriteComponent* sprite = player->getComponent<SpriteComponent>();
    PhysicsComponent* physics = player->getComponent<PhysicsComponent>();
    ScriptComponent* scriptComponent = player->getComponent<ScriptComponent>();

    scriptComponent->addScript(std::make_shared<PlayerAnimationScript>());
    scriptComponent->addScript(std::make_shared<PlayerMovementScript>());
    scriptComponent->addScript(std::make_shared<PlayerInputScript>());
    scriptComponent->addScript(std::make_shared<PlayerCollisionScript>());

    physics->damping = 0.1f;

    // Return the created player entity
    return player;
}

