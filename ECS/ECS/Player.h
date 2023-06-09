#pragma once
#include <SDL.h>
#include "ECS.h"
#include "PCM.h"

using namespace PC;

std::shared_ptr<Entity> createPlayerPrefab(SDL_Renderer* renderer) {
    // Create a new entity
    std::shared_ptr<Entity> player = Entity::create();


    // Add necessary components
    player->addComponent<TransformComponent>(Vector2f(320, 240), 0.0f, Vector2f(3.f, 3.f));
    player->addComponent<SpriteComponent>(renderer,
        "Assets/mystic_woods_2.1/sprites/characters/player.png");
    player->addComponent<VelocityComponent>(100, 100);  
    player->addComponent<InputComponent>();
    player->addComponent<UpdateComponent>();
    player->addComponent<BoxColliderComponent>();

    // Get the InputComponent and bind keys to commands
    InputComponent* input = player->getComponent<InputComponent>();
    VelocityComponent* velocity = player->getComponent<VelocityComponent>();
    UpdateComponent* update = player->getComponent<UpdateComponent>();
    TransformComponent* transform = player->getComponent<TransformComponent>();
    SpriteComponent* sprite = player->getComponent<SpriteComponent>();

    // Define animation states
    sprite->addAnimationState("idleFront", AnimationState(6*0, 6, 100));    
    sprite->addAnimationState("idleRight", AnimationState(6*1, 6, 100));   
    sprite->addAnimationState("idleLeft", AnimationState(6*1, 6, 100, SDL_FLIP_HORIZONTAL));
    sprite->addAnimationState("idleBack", AnimationState(6*2, 6, 100));    
    sprite->addAnimationState("walkingFront", AnimationState(6*3, 6, 100));    
    sprite->addAnimationState("walkingRight", AnimationState(6*4, 6, 100));    
    sprite->addAnimationState("walkingLeft", AnimationState(6*4, 6, 100, SDL_FLIP_HORIZONTAL));    
    sprite->addAnimationState("walkingBack", AnimationState(6*5, 6, 100));    
    sprite->addAnimationState("attackingFront", AnimationState(6*6, 4, 100));
    sprite->addAnimationState("attackingRight", AnimationState(6*6 + 4, 4, 100));    
    sprite->addAnimationState("attackingLeft", AnimationState(6*6 + 4, 4, 100, SDL_FLIP_HORIZONTAL));
    sprite->addAnimationState("attackingBack", AnimationState(6 * 6 + 4 * 2, 4, 100));
    sprite->addAnimationState("death", AnimationState(6 * 6 + 4 * 3, 4, 100));

    // Set the animation
    sprite->setAnimationState("idleFront");

    //upwards movement
    input->bindKeyDown(SDLK_w, [velocity, sprite](Entity& entity) {        
        if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
            && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
            velocity->dy = -velocity->dyMax;
        }        
    });

    input->bindKeyUp(SDLK_w, [velocity, sprite](Entity& entity) {
        velocity->dy = 0;
    });

    //downwards movement
    input->bindKeyDown(SDLK_s, [velocity, sprite](Entity& entity) {        
        if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
            && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
            velocity->dy = velocity->dyMax;
        }
    });

    input->bindKeyUp(SDLK_s, [velocity, sprite](Entity& entity) {
        velocity->dy = 0;
    });

    //left movement
    input->bindKeyDown(SDLK_a, [velocity, sprite](Entity& entity) {        
        if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
            && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
            velocity->dx = -velocity->dxMax;
        }
    });

    input->bindKeyUp(SDLK_a, [velocity, sprite](Entity& entity) {
        velocity->dx = 0;
    });

    //right movement
    input->bindKeyDown(SDLK_d, [velocity, sprite](Entity& entity) {        
        if (sprite->currentState != "attackingRight" && sprite->currentState != "attackingLeft"
            && sprite->currentState != "attackingFront" && sprite->currentState != "attackingBack") {
            velocity->dx = velocity->dxMax;
        }
    });

    input->bindKeyUp(SDLK_d, [velocity, sprite](Entity& entity) {
        velocity->dx = 0;
    });

    //mouse
    input->bindMouseButtonDown(SDL_BUTTON_LEFT, [velocity, sprite](Entity& entity) {
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
        

    });

    //update
    update->addUpdateFunction([transform, velocity](Entity& entity, float deltaTime) {
        transform->setPosition({ transform->getPosition().x + velocity->dx * deltaTime,
                                transform->getPosition().y + velocity->dy * deltaTime });
    });

    update->addUpdateFunction([sprite, transform, velocity](Entity& entity, float deltaTime) {
        // Determine the direction of movement based on the velocity
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
            sprite->setAnimationState(!sprite->animationPlaying&& sprite->currentState == "attackingFront" ? "idleFront" : "walkingFront");
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
    });

    // Return the created player entity
    return player;
}

