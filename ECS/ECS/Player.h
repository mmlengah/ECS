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

    //add wasd controlls? 
    input->bindKeyDown(SDLK_w, [velocity](Entity& entity) {
        std::cout << "pressed" << std::endl;
        velocity->dy = velocity->dyMax;
    });

    //update
    update->addUpdateFunction([transform, velocity](Entity& entity, float deltaTime) {
        transform->position.x += velocity->dx * deltaTime;
        transform->position.y += velocity->dy * deltaTime;
    });

    // Return the created player entity
    return player;
}

