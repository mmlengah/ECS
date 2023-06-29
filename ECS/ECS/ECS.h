#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <SDL.h>
#include <SDL_image.h>
#include "PCM.h"
#include "Camera.h"

using namespace PC;

class Entity;
class Component;
class System;

class Entity : public std::enable_shared_from_this<Entity> {
private:
    inline static std::vector<std::shared_ptr<Entity>> allEntities = {};
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

    // Make constructor private so it can't be called directly
    Entity() {}

public:
    template<typename T, typename... TArgs>
    void addComponent(TArgs&&... args) {
        components[typeid(T)] = std::make_unique<T>(std::forward<TArgs>(args)...);
    }

    template<typename T>
    T* getComponent() {
        auto iter = components.find(typeid(T));
        if (iter != components.end()) {
            return dynamic_cast<T*>(iter->second.get());
        }
        return nullptr;
    }

    static std::vector<std::shared_ptr<Entity>>& getAllEntities() {
        return allEntities;
    }

    // Factory method to create a new Entity
    static std::shared_ptr<Entity> create() {
        std::shared_ptr<Entity> entity(new Entity());
        allEntities.push_back(entity);
        return entity;
    }
};

class Component {
public:
    Entity* entity;
    virtual ~Component() {}
};

class System {
public:
    std::vector<Entity*> entities;

    virtual void tryAddEntity(Entity* entity) = 0;
};

class SystemManager {
public:
    std::vector<std::unique_ptr<System>> systems;

    template<typename T, typename... TArgs>
    T& registerSystem(TArgs&&... args) {
        T* t = new T(std::forward<TArgs>(args)...);
        systems.emplace_back(t);
        return *t;
    }

    void addAllEntitiesToSystems(std::vector<std::shared_ptr<Entity>>& allEntities) {
        for (auto& entity : allEntities) {
            addEntityToSystems(entity.get());
        }
    }
private:
    void addEntityToSystems(Entity* entity) {
        for (auto& system : systems) {
            system->tryAddEntity(entity);
        }
    }
};

class TransformComponent : public Component {
public:
    Vector2f position;
    float rotation;
    Vector2f scale;
    Matrix3x3<float> transformMatrix;

    TransformComponent(Vector2f position = Vector2f(0.0f, 0.0f), float rotation = 0.0f, Vector2f scale = Vector2f(1.0f, 1.0f)) : position(position),
        rotation(rotation), scale(scale)
    {
        updateTransformMatrix();
    }

    void updateTransformMatrix()
    {
        transformMatrix = Matrix3x3<float>::Matrix3x3FromTranslation(position) *
            Matrix3x3<float>::Matrix3x3FromRotation(rotation) *
            Matrix3x3<float>::Matrix3x3FromScale(scale);
    }    
};

class SpriteComponent : public Component {
public:
    SDL_Texture* spriteSheet;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int frameCount;
    int currentFrame;
    int frameWidth;
    int xOffset;
    Uint32 lastFrameTime;
    Uint32 frameDelay;

    SpriteComponent(SDL_Renderer* renderer, const char* path, SDL_Rect spriteRect, int frames,
        int frameWidth, Uint32 frameDelay)
        : srcRect(spriteRect), dstRect({ 0, 0, spriteRect.w * 2, spriteRect.h * 2 }),
        frameCount(frames), currentFrame(0), frameWidth(frameWidth), xOffset(spriteRect.x),
        lastFrameTime(0), frameDelay(frameDelay) {
        spriteSheet = IMG_LoadTexture(renderer, path);
    }

    ~SpriteComponent() {
        if (spriteSheet) {
            SDL_DestroyTexture(spriteSheet);
        }
    }

    void setAnimation(int yOffset, int frames, Uint32 frameDelay) {
        srcRect.y = yOffset;
        this->frameCount = frames;
        this->frameDelay = frameDelay;
        currentFrame = 0;
    }

    void render(SDL_Renderer* renderer, int x, int y) {
        dstRect.x = x - dstRect.w;
        dstRect.y = y - dstRect.h;
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &dstRect);
    }

    void nextFrame() {
        // Get the current time.
        Uint32 currentTime = SDL_GetTicks();

        // If enough time has passed since the last frame...
        if (currentTime > lastFrameTime + frameDelay) {
            // Update the frame.
            currentFrame = (currentFrame + 1) % frameCount;
            srcRect.x = xOffset + (currentFrame * frameWidth);
            // Set the time of this frame.
            lastFrameTime = currentTime;
        }
    }
};

class VelocityComponent : public Component {
public:
    int dx;
    int dy;

    int dxMax;
    int dyMax;

    VelocityComponent(int dx, int dy) : dx(0), dy(0), dxMax(dx), dyMax(dy) {}
};

class InputComponent : public Component {
public:
    using FunctionId = int;

    std::unordered_map<SDL_Keycode, std::unordered_map<FunctionId, std::function<void(Entity&)>>> keyDownMapping;
    std::unordered_map<SDL_Keycode, std::unordered_map<FunctionId, std::function<void(Entity&)>>> keyUpMapping;
    std::unordered_map<SDL_Keycode, std::unordered_map<FunctionId, std::function<void(Entity&)>>> keyHoldMapping;
    std::unordered_map<SDL_Keycode, bool> keyState; // This will store whether each key is currently down
    FunctionId nextId = 0;

    FunctionId bindKeyDown(SDL_Keycode key, std::function<void(Entity&)> command) {
        FunctionId id = nextId++;
        keyDownMapping[key][id] = command;
        return id;
    }

    FunctionId bindKeyUp(SDL_Keycode key, std::function<void(Entity&)> command) {
        FunctionId id = nextId++;
        keyUpMapping[key][id] = command;
        return id;
    }

    FunctionId bindKeyHold(SDL_Keycode key, std::function<void(Entity&)> command) {
        FunctionId id = nextId++;
        keyHoldMapping[key][id] = command;
        return id;
    }

    void unbindKey(SDL_Keycode key) {
        keyDownMapping.erase(key);
        keyUpMapping.erase(key);
    }

    void unbindFunction(SDL_Keycode key, FunctionId id) {
        auto itDown = keyDownMapping.find(key);
        if (itDown != keyDownMapping.end()) {
            itDown->second.erase(id);
        }

        auto itUp = keyUpMapping.find(key);
        if (itUp != keyUpMapping.end()) {
            itUp->second.erase(id);
        }
    }

    void setKeyDown(SDL_Keycode key) {
        keyState[key] = true;
    }

    void setKeyUp(SDL_Keycode key) {
        keyState[key] = false;
    }
};

class SquareComponent : public Component {
public:
    SDL_Rect dstRect;
    SDL_Color color;

    SquareComponent(SDL_Rect dstRect, SDL_Color color) : dstRect(dstRect), color(color) {}
};

class UpdateComponent : public Component {
public:
    std::vector<std::function<void(Entity&, float)>> onUpdate;

    void addUpdateFunction(std::function<void(Entity&, float)> updateFn) {
        onUpdate.push_back(updateFn);
    }
};

class RenderSystem : public System {
public:
    SDL_Renderer* renderer;
    Camera* cam;
    RenderSystem(SDL_Renderer* renderer, Camera* cam) : renderer(renderer), cam(cam) {}

    void update() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); //error here

        for (Entity* entity : entities) {
            TransformComponent* transform = entity->getComponent<TransformComponent>();
            SpriteComponent* sprite = entity->getComponent<SpriteComponent>();            
            SquareComponent* shape = entity->getComponent<SquareComponent>();
            if (sprite) {
                Matrix3x3<float> worldSpace = cam->getTransformMatrix() * transform->transformMatrix;
                
                Vector2f pos = worldSpace.getTranslation();
                Vector2f scale = worldSpace.getScale();
                float rot = worldSpace.getRotation();

                SDL_Rect temp = sprite->dstRect;

                temp.w = static_cast<int>(temp.w * scale.x);
                temp.h = static_cast<int>(temp.h * scale.y);
                temp.x = static_cast<int>(pos.x) - temp.w / 2;
                temp.y = static_cast<int>(pos.y) - temp.h / 2;

                SDL_RenderCopyEx(renderer, sprite->spriteSheet, &(sprite->srcRect), &temp,
                    transform->rotation, nullptr, SDL_FLIP_NONE);
                sprite->nextFrame();
            }
            else if (shape) {
                // render shape
                Matrix3x3<float> worldSpace = cam->getTransformMatrix() * transform->transformMatrix;

                Vector2f pos = worldSpace.getTranslation();
                Vector2f scale = worldSpace.getScale();

                SDL_Rect temp = shape->dstRect;

                temp.w = static_cast<int>(temp.w * scale.x);
                temp.h = static_cast<int>(temp.h * scale.y);
                temp.x = static_cast<int>(pos.x) - temp.w / 2;
                temp.y = static_cast<int>(pos.y) - temp.h / 2;

                SDL_SetRenderDrawColor(renderer, shape->color.r, shape->color.g, shape->color.b, shape->color.a);
                SDL_RenderFillRect(renderer, &temp);
            }
        }

        SDL_RenderPresent(renderer);
    }

    void tryAddEntity(Entity* entity) override {
        if (entity->getComponent<TransformComponent>() && entity->getComponent<SpriteComponent>() || entity->getComponent<SquareComponent>()) {
            entities.push_back(entity);
        }
    }
};

class InputSystem : public System {
public:
    void update(SDL_Event& event) {
        for (Entity* entity : entities) {
            InputComponent* input = entity->getComponent<InputComponent>();
            if (input) {
                // Check for keydown and keyup events
                if (event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    input->setKeyDown(key);
                    auto it = input->keyDownMapping.find(key);
                    if (it != input->keyDownMapping.end()) {
                        for (auto& fn : it->second) {
                            if (fn.second) fn.second(*entity);
                        }
                    }
                }
                else if (event.type == SDL_KEYUP) {
                    SDL_Keycode key = event.key.keysym.sym;
                    input->setKeyUp(key);
                    auto it = input->keyUpMapping.find(key);
                    if (it != input->keyUpMapping.end()) {
                        for (auto& fn : it->second) {
                            if (fn.second) fn.second(*entity);
                        }
                    }
                }

                // Check for key hold events
                for (auto& keyStatePair : input->keyState) {
                    if (keyStatePair.second) { // If the key is down
                        auto it = input->keyHoldMapping.find(keyStatePair.first);
                        if (it != input->keyHoldMapping.end()) {
                            for (auto& fn : it->second) {
                                if (fn.second) fn.second(*entity);
                            }
                        }
                    }
                }
            }
        }
    }

    void tryAddEntity(Entity* entity) override {
        if (entity->getComponent<InputComponent>()) {
            entities.push_back(entity);
        }
    }
};

class UpdateSystem : public System {
public:
    void update(float deltaTime = 1) {
        for (Entity* entity : entities) {
            UpdateComponent* update = entity->getComponent<UpdateComponent>();
            if (update) {
                for (auto& updateFunction : update->onUpdate) {
                    updateFunction(*entity, deltaTime);
                }
            }
        }
    }

    void tryAddEntity(Entity* entity) override {
        if (entity->getComponent<UpdateComponent>()) {
            entities.push_back(entity);
        }
    }
};