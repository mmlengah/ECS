#pragma once
#include <unordered_map>
#include <typeindex>

class Entity;

class Component {
public:
    Entity* entity; // Pointer to the entity that the component is associated with
    virtual ~Component() = default;
};

class TransformComponent : public Component {
public:
    // Add member variables and functions specific to the TransformComponent
};

class SpriteComponent : public Component {
public:
    // Add member variables and functions specific to the SpriteComponent
};

class InputComponent : public Component {
public:
    // Add member variables and functions specific to the InputComponent
};

class Entity {
private:
    std::unordered_map<std::type_index, Component*> components;

public:
    ~Entity() {
        for (auto& component : components) {
            delete component.second;
        }
    }

    template<typename T, typename... Args>
    void addComponent(Args&&... args) {
        T* comp = new T(std::forward<Args>(args)...);
        components[typeid(T)] = comp;
    }

    template<typename T>
    T* getComponent() {
        return dynamic_cast<T*>(components[typeid(T)]);
    }
};
