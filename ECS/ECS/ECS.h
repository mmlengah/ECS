#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <SDL.h>
#include "PCM.h"

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
    TransformComponent(Vector2f position = Vector2f(0.0f, 0.0f), float rotation = 0.0f, Vector2f scale = Vector2f(1.0f, 1.0f)) : position(position),
        rotation(rotation), scale(scale)
    {}
};

class SpriteComponent : public Component {
public:
    SDL_Rect rect;
    SpriteComponent(SDL_Rect rect) : rect(rect) {}
};

class RenderSystem : public System {
public:
    SDL_Renderer* renderer;

    RenderSystem(SDL_Renderer* renderer) : renderer(renderer) {}

    void update() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); //error here

        for (Entity* entity : entities) {
            TransformComponent* transform = entity->getComponent<TransformComponent>();
            SpriteComponent* sprite = entity->getComponent<SpriteComponent>();

            if (transform) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect temp = sprite->rect;
                temp.x = static_cast<int>(transform->position.x);
                temp.y = static_cast<int>(transform->position.y);
                temp.w = static_cast<int>(temp.w * transform->scale.x);
                temp.h = static_cast<int>(temp.h * transform->scale.y);
                SDL_RenderFillRect(renderer, &temp);
                //SDL_RenderCopyEx(renderer, texture, nullptr, &temp, transform->rotation, nullptr, SDL_FLIP_NONE);
            }
        }

        SDL_RenderPresent(renderer);
    }

    void tryAddEntity(Entity* entity) override {
        if (entity->getComponent<TransformComponent>() && entity->getComponent<SpriteComponent>()) {
            entities.push_back(entity);
        }
    }
};

