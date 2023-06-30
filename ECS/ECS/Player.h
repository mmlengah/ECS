#pragma once
#include <SDL.h>
#include "ECS.h"
#include "PCM.h"

using namespace PC;

std::shared_ptr<Entity> createPlayerPrefab(SDL_Renderer* renderer) {
    // Create a new entity
    std::shared_ptr<Entity> player = Entity::create();

    SDL_Rect spriteRect = { 18, 22, 13, 21 };

    // Add necessary components
    player->addComponent<TransformComponent>(Vector2f(320, 240), 0.0f, Vector2f(1.5f, 1.5f));
    player->addComponent<SpriteComponent>(renderer,
        "Assets/mystic_woods_2.1/sprites/characters/player.png",
        spriteRect, 6, 48, 100);
    player->addComponent<VelocityComponent>(100, 100);  
    player->addComponent<InputComponent>();
    player->addComponent<UpdateComponent>();

    // Get the InputComponent and bind keys to commands
    InputComponent* input = player->getComponent<InputComponent>();
    VelocityComponent* velocity = player->getComponent<VelocityComponent>();
    UpdateComponent* update = player->getComponent<UpdateComponent>();
    TransformComponent* transform = player->getComponent<TransformComponent>();
    SpriteComponent* sprite = player->getComponent<SpriteComponent>();

    // Define animation states
    sprite->addAnimationState("idle", AnimationState(22, 6, 60));
    sprite->addAnimationState("idleLeft", AnimationState(70, 4, 60, SDL_FLIP_HORIZONTAL));
    sprite->addAnimationState("idleRight", AnimationState(70, 6, 60));    
    sprite->addAnimationState("attackingRight", AnimationState(359, 6, 60));

    // Set the animation
    sprite->setAnimationState("idleLeft");

    //upwards movement
    input->bindKeyDown(SDLK_w, [velocity](Entity& entity) {
        velocity->dy = -velocity->dyMax;
    });

    input->bindKeyUp(SDLK_w, [velocity](Entity& entity) {
        velocity->dy = 0;
    });

    //downwards movement
    input->bindKeyDown(SDLK_s, [velocity](Entity& entity) {
        velocity->dy = velocity->dyMax;
    });

    input->bindKeyUp(SDLK_s, [velocity](Entity& entity) {
        velocity->dy = 0;
    });

    //left movement
    input->bindKeyDown(SDLK_a, [velocity](Entity& entity) {
        velocity->dx = -velocity->dxMax;
    });

    input->bindKeyUp(SDLK_a, [velocity](Entity& entity) {
        velocity->dx = 0;
    });

    //right movement
    input->bindKeyDown(SDLK_d, [velocity](Entity& entity) {
        velocity->dx = velocity->dxMax;
    });

    input->bindKeyUp(SDLK_d, [velocity](Entity& entity) {
        velocity->dx = 0;
    });

    //mouse
    input->bindMouseButtonDown(SDL_BUTTON_LEFT, [sprite](Entity& entity) {
        if (sprite->currentState != "attackingRight") {
            sprite->setAnimationState("attackingRight");
        }
    });

    //update
    update->addUpdateFunction([transform, velocity](Entity& entity, float deltaTime) {
        transform->setPosition({ transform->getPosition().x + velocity->dx * deltaTime,
                                transform->getPosition().y + velocity->dy * deltaTime });
    });

    //update animations
    update->addUpdateFunction([sprite, transform, velocity](Entity& entity, float deltaTime) {
        if (!sprite->animationPlaying && sprite->currentState == "attackingRight") {
            sprite->setAnimationState("idleRight");
        }

    });

    // Return the created player entity
    return player;
}

