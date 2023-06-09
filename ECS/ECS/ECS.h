#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include "PCM.h"
#include "Camera.h"

/*
TODO:
Collision
Sound
Light
Particles
Physics
*/

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
    template <typename T, typename... TArgs>
    T* addComponent(TArgs&&... args) {
        T* comp = new T(std::forward<TArgs>(args)...);
        comp->owner = this; // Set the owner of the component to this entity
        components[std::type_index(typeid(T))] = std::unique_ptr<T>(comp);
        return comp;
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
    Entity* owner;
    virtual ~Component() {}

    Component() : owner(nullptr) {}

    template <typename T>
    T* getComponent() const {
        return owner->getComponent<T>();
    }
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
    TransformComponent(Vector2f position = Vector2f(0.0f, 0.0f),
        float rotation = 0.0f, Vector2f scale = Vector2f(1.0f, 1.0f)) {
        setPosition(position);
        setRotation(rotation);
        setScale(scale);
    }

    Vector2f getPosition() {
        return position;
    }

    void setPosition(const Vector2f& pos) {
        position = pos;
        updateTransformMatrix();
    }

    float getRotation() {
        return rotation;
    }

    void setRotation(float rot) {
        rotation = rot;
        updateTransformMatrix();
    }

    Vector2f getScale() {
        return scale;
    }

    void setScale(const Vector2f& scl) {
        scale = scl;
        updateTransformMatrix();
    }

    Matrix3x3<float> getTransformMatrix() {
        return transformMatrix;
    }

    void updateTransformMatrix() {
        transformMatrix = Matrix3x3<float>::Matrix3x3FromTranslation(position) *
            Matrix3x3<float>::Matrix3x3FromRotation(rotation) *
            Matrix3x3<float>::Matrix3x3FromScale(scale);
    }

    void setWorldSpaceMatrix(const Matrix3x3<float>& matrix) {
        worldSpaceMatrix = matrix;
    }

    Matrix3x3<float> getWorldSpaceMatrix() {
        return worldSpaceMatrix;
    }

    Vector2f getWorldSpacePosition() {
        return worldSpaceMatrix.getTranslation();
    }

    float getWorldSpaceRotation() {
        return worldSpaceMatrix.getRotation();
    }

private:
    Vector2f position;
    float rotation;
    Vector2f scale;
    Matrix3x3<float> transformMatrix;
    Matrix3x3<float> worldSpaceMatrix;
};

class AnimationState {
public:
    int beginFrameIndex;
    int frameCount;
    Uint32 frameDelay;
    SDL_RendererFlip flip;

    AnimationState(int beginFrameIndex = 0, int frameCount = 0, Uint32 frameDelay = 60, SDL_RendererFlip flip = SDL_FLIP_NONE)
        : beginFrameIndex(beginFrameIndex), frameCount(frameCount), frameDelay(frameDelay), flip(flip) {}
};

class SpriteHandler {
public:
    SpriteHandler() : Y_THRESHOLD(10) {}
    std::vector<SDL_Rect> CCL(const std::string& imagePath, int tolerance = 12, int yThreshold = 10) {
        Y_THRESHOLD = yThreshold;
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << IMG_GetError() << "\n";
            return {};
        }

        std::vector<std::vector<int>> labels(surface->w, std::vector<int>(surface->h, 0));
        std::vector<SDL_Rect> bounds(surface->w * surface->h, { surface->w, surface->h, 0, 0 });

        int currentLabel = 1;

        for (int y = 0; y < surface->h; ++y) {
            for (int x = 0; x < surface->w; ++x) {
                if (!isBackground(surface, x, y)) {
                    std::vector<int> neighborLabels;

                    for (int i = -tolerance; i <= tolerance; i++) {
                        for (int j = -tolerance; j <= tolerance; j++) {
                            int nx = x + i;
                            int ny = y + j;
                            if (nx >= 0 && ny >= 0 && nx < surface->w && ny < surface->h) {
                                if (labels[nx][ny] != 0) {
                                    neighborLabels.push_back(labels[nx][ny]);
                                }
                            }
                        }
                    }

                    int label;
                    if (neighborLabels.empty()) {
                        label = currentLabel++;
                        bounds[label].x = x;
                        bounds[label].y = y;
                        bounds[label].w = x;
                        bounds[label].h = y;
                    }
                    else {
                        label = *min_element(neighborLabels.begin(), neighborLabels.end());

                        for (int otherLabel : neighborLabels) {
                            if (otherLabel != label) {
                                // Merge bounding rectangles
                                bounds[label].x = std::min(bounds[label].x, bounds[otherLabel].x);
                                bounds[label].y = std::min(bounds[label].y, bounds[otherLabel].y);
                                bounds[label].w = std::max(bounds[label].w, bounds[otherLabel].w);
                                bounds[label].h = std::max(bounds[label].h, bounds[otherLabel].h);

                                // Replace all instances of 'otherLabel' in the image with 'label'
                                for (auto& row : labels) {
                                    std::replace(row.begin(), row.end(), otherLabel, label);
                                }
                            }
                        }
                    }

                    // Update bounding rectangle
                    bounds[label].x = std::min(bounds[label].x, x);
                    bounds[label].y = std::min(bounds[label].y, y);
                    bounds[label].w = std::max(bounds[label].w, x);
                    bounds[label].h = std::max(bounds[label].h, y);

                    labels[x][y] = label;
                }
            }
        }

        // Transform 'bounds' to a list of 'sprites'
        std::vector<SDL_Rect> sprites;
        for (int i = 1; i < currentLabel; ++i) {
            sprites.push_back({ bounds[i].x, bounds[i].y, bounds[i].w - bounds[i].x + 1, bounds[i].h - bounds[i].y + 1 });
        }

        SDL_FreeSurface(surface);
        std::sort(sprites.begin(), sprites.end(), [this](const SDL_Rect& a, const SDL_Rect& b) {return compareSprites(a, b); });

        return sprites;
    }
private:
    struct Point {
        int x;
        int y;

        Point() : x(0), y(0) {}

        friend std::ostream& operator<<(std::ostream& os, const Point& point) {
            os << "x: " << point.x << ", y: " << point.y;
            return os;
        }
    };

    bool isBackground(SDL_Surface* surface, int x, int y) {
        // This function returns true if the pixel at (x, y) is transparent.

        // Make sure we're within the surface bounds
        if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) {
            return true;
        }

        // First, get a pointer to the pixel.
        Uint32* pixels = (Uint32*)surface->pixels;
        Uint32 pixel = pixels[(y * surface->w) + x];

        // Then, get the RGBA values of the pixel.
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

        // Return true if the alpha value is 0 (fully transparent).
        return a == 0;
    }

    bool compareSprites(const SDL_Rect& a, const SDL_Rect& b) {
        // If the y-coordinates are within the threshold, sort by the x-coordinate
        if (std::abs(a.y - b.y) <= Y_THRESHOLD) {
            return a.x < b.x;
        }
        // Otherwise, sort by the y-coordinate
        return a.y < b.y;
    }
private:
    int Y_THRESHOLD;
};

class SpriteComponent : public Component {
public:
    SDL_Texture* spriteSheet;
    std::vector<SDL_Rect> frames;
    SDL_Rect srcRect;
    int startingFrame;
    int currentFrame;
    int frameCount;
    Uint32 lastFrameTime;
    Uint32 frameDelay;
    SDL_RendererFlip flip;
    std::unordered_map<std::string, AnimationState> animationStates;
    std::string currentState;
    bool animationPlaying;

    SpriteComponent(SDL_Renderer* renderer, const char* path, int tolerance = 12, 
        int yThreshold = 10)
        : startingFrame(0), currentFrame(0), lastFrameTime(0), 
        frameDelay(100), flip(SDL_FLIP_NONE), animationPlaying(false) {

        spriteSheet = IMG_LoadTexture(renderer, path);
        if (!spriteSheet) {
            std::cerr << "Failed to load texture: " << IMG_GetError() << "\n";
        }

        spriteHandler = new SpriteHandler();
        frames = spriteHandler->CCL(path, tolerance, yThreshold);
        std::cout << "frames: " << frames.size() << std::endl;
        frameCount = static_cast<int>(frames.size());
        srcRect = frames[0];
    }

    ~SpriteComponent() {
        if (spriteSheet) {
            SDL_DestroyTexture(spriteSheet);
        }
        delete spriteHandler;
    }

    void addAnimationState(const std::string& stateName, AnimationState state) {
        animationStates[stateName] = state;
    }

    void setAnimationState(const std::string& stateName) {
        if (currentState == stateName) { return; }
        auto it = animationStates.find(stateName);
        if (it != animationStates.end()) {
            animationPlaying = true;
            currentState = stateName;
            AnimationState& state = it->second;
            startingFrame = state.beginFrameIndex;
            currentFrame = state.beginFrameIndex;            
            frameCount = state.frameCount;
            frameDelay = state.frameDelay;
            flip = state.flip; 
        }
        else {
            std::cerr << "No such animation state exists: " << stateName << std::endl;
        }
    }

    void nextFrame() {
        // Get the current time.
        Uint32 currentTime = SDL_GetTicks();

        // If enough time has passed since the last frame...
        if (currentTime > lastFrameTime + frameDelay) {
            //std::cout << currentFrame << std::endl;

            currentFrame = (currentFrame - startingFrame + 1) % frameCount + startingFrame;

            srcRect = frames[currentFrame];

            // Set the time of this frame.
            lastFrameTime = currentTime;

            auto it = animationStates.find(currentState);
            if (it != animationStates.end()) {
                AnimationState& state = it->second;
                // Check if we've reached the end of the animation.
                if (currentFrame - startingFrame + 1 >= state.frameCount) {
                    animationPlaying = false;
                    //std::cout << currentFrame << std::endl;
                    currentFrame = state.beginFrameIndex;                    
                }
            }
        }
    }

    SDL_Rect getWorldSpaceRect() {
        TransformComponent* transform = owner->getComponent<TransformComponent>();
        Matrix3x3f m = transform->getWorldSpaceMatrix();

        Vector2f pos = m.getTranslation();
        Vector2f scale = m.getScale();

        SDL_Rect temp = { static_cast<int>(pos.x), static_cast<int>(pos.y),
            static_cast<int>(srcRect.w * scale.x), static_cast<int>(srcRect.h * scale.y) };
        
        temp.x = static_cast<int>(pos.x) - temp.w / 2;
        temp.y = static_cast<int>(pos.y) - temp.h / 2;
        
        return temp;
    }
private:
    SpriteHandler* spriteHandler;
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
    std::unordered_map<uint8_t, std::unordered_map<FunctionId, std::function<void(Entity&)>>> mouseButtonDownMapping;
    std::unordered_map<uint8_t, std::unordered_map<FunctionId, std::function<void(Entity&)>>> mouseButtonUpMapping;
    FunctionId nextId = 0;

    FunctionId bindMouseButtonDown(uint8_t button, std::function<void(Entity&)> command) {
        FunctionId id = nextId++;
        mouseButtonDownMapping[button][id] = command;
        return id;
    }

    FunctionId bindMouseButtonUp(uint8_t button, std::function<void(Entity&)> command) {
        FunctionId id = nextId++;
        mouseButtonUpMapping[button][id] = command;
        return id;
    }

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

    void unbindMouseButton(uint8_t button) {
        mouseButtonDownMapping.erase(button);
        mouseButtonUpMapping.erase(button);
    }

    void unbindMouseFunction(uint8_t button, FunctionId id) {
        auto itDown = mouseButtonDownMapping.find(button);
        if (itDown != mouseButtonDownMapping.end()) {
            itDown->second.erase(id);
        }

        auto itUp = mouseButtonUpMapping.find(button);
        if (itUp != mouseButtonUpMapping.end()) {
            itUp->second.erase(id);
        }
    }
};

struct Rectangle {
    int width;
    int height;

    Rectangle() : width(0), height(0) {}

    Rectangle(int w, int h) : width(w), height(h) {}

    friend std::ostream& operator<<(std::ostream& os, const Rectangle& rect) {
        os << "Width: " << rect.width << ", Height: " << rect.height;
        return os;
    }
};

class SquareComponent : public Component {
public:
    Rectangle rect;
    SDL_Color color;

    SquareComponent(Rectangle rect, SDL_Color color) : rect(rect), color(color) {}

    SDL_Rect getWorldSpaceRect() {
        TransformComponent* transform = owner->getComponent<TransformComponent>();
        Matrix3x3f m = transform->getWorldSpaceMatrix();

        Vector2f pos = m.getTranslation();
        Vector2f scale = m.getScale();

        SDL_Rect temp = { static_cast<int>(pos.x), static_cast<int>(pos.y),
            static_cast<int>(rect.width * scale.x), static_cast<int>(rect.height * scale.y) };
        
        temp.x = static_cast<int>(pos.x) - temp.w / 2;
        temp.y = static_cast<int>(pos.y) - temp.h / 2;
        
        return temp;
    }
};

class BoxColliderComponent : public Component {
public:
    BoxColliderComponent() : customCollider(false) {}

    BoxColliderComponent(Rectangle rect) : rect(rect), customCollider(false) {}

    SDL_Rect getWorldSpaceRect() {
        SpriteComponent* sprite = owner->getComponent<SpriteComponent>();
        SquareComponent* square = owner->getComponent<SquareComponent>();

        if (customCollider) {            
            TransformComponent* transform = owner->getComponent<TransformComponent>();
            Matrix3x3f m = transform->getWorldSpaceMatrix();

            Vector2f pos = m.getTranslation();
            Vector2f scale = m.getScale();

            SDL_Rect temp = { static_cast<int>(pos.x), static_cast<int>(pos.y),
                static_cast<int>(rect.width * scale.x), static_cast<int>(rect.height * scale.y) };

            return temp;
        }
        else if (sprite) {
            return sprite->getWorldSpaceRect();
        }
        else if (square) {
            return square->getWorldSpaceRect();
        }

        return { 0, 0, 0, 0 };
    }

private:
    Rectangle rect;
    bool customCollider;
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

    RenderSystem(SDL_Renderer* renderer) : renderer(renderer) {}

    void update() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); //error here

        for (auto it = entities.rbegin(); it != entities.rend(); ++it) {
            Entity* entity = *it;
            TransformComponent* transform = entity->getComponent<TransformComponent>();
            SpriteComponent* sprite = entity->getComponent<SpriteComponent>();            
            SquareComponent* shape = entity->getComponent<SquareComponent>();
            if (sprite) {
                SDL_Rect temp = sprite->getWorldSpaceRect();

                float rot = transform->getWorldSpaceRotation();

                SDL_RenderCopyEx(renderer, sprite->spriteSheet, &(sprite->srcRect), &temp,
                    transform->getRotation(), nullptr, sprite->flip);
                sprite->nextFrame();

                BoxColliderComponent* boxCollider = entity->getComponent<BoxColliderComponent>();
                if (boxCollider) {
                    SDL_Rect rect = boxCollider->getWorldSpaceRect();

                    SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
            else if (shape) {
                SDL_Rect temp = shape->getWorldSpaceRect();

                float rot = transform->getWorldSpaceRotation();

                SDL_SetRenderDrawColor(renderer, shape->color.r, shape->color.g,
                    shape->color.b, shape->color.a);
                SDL_RenderFillRect(renderer, &temp);

                BoxColliderComponent* boxCollider = entity->getComponent<BoxColliderComponent>();
                if (boxCollider) {
                    SDL_Rect rect = boxCollider->getWorldSpaceRect();

                    SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
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
                    auto it = input->keyDownMapping.find(key);
                    if (it != input->keyDownMapping.end()) {
                        for (auto& fn : it->second) {
                            if (fn.second) fn.second(*entity);
                        }
                    }
                }
                else if (event.type == SDL_KEYUP) {
                    SDL_Keycode key = event.key.keysym.sym;
                    auto it = input->keyUpMapping.find(key);
                    if (it != input->keyUpMapping.end()) {
                        for (auto& fn : it->second) {
                            if (fn.second) fn.second(*entity);
                        }
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    uint8_t button = event.button.button;
                    auto it = input->mouseButtonDownMapping.find(button);
                    if (it != input->mouseButtonDownMapping.end()) {
                        for (auto& fn : it->second) {
                            if (fn.second) fn.second(*entity);
                        }
                    }
                }
                else if (event.type == SDL_MOUSEBUTTONUP) {
                    uint8_t button = event.button.button;
                    auto it = input->mouseButtonUpMapping.find(button);
                    if (it != input->mouseButtonUpMapping.end()) {
                        for (auto& fn : it->second) {
                            if (fn.second) fn.second(*entity);
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

class WorldSpaceSystem : public System {
public:
    Camera* cam;

    WorldSpaceSystem(Camera* cam) : cam(cam) {}

    void update() {
        for (auto it = entities.begin(); it != entities.end(); ++it) {
            Entity* entity = *it;
            TransformComponent* transform = entity->getComponent<TransformComponent>();
            if (transform) {
                Matrix3x3<float> worldSpace = cam->getTransformMatrix() *
                    transform->getTransformMatrix();
                transform->setWorldSpaceMatrix(worldSpace);
            }
        }
    }

    void tryAddEntity(Entity* entity) override {
        if (entity->getComponent<TransformComponent>()) {
            entities.push_back(entity);
        }
    }
};

class CollisionSystem : public System {
public:
    void update() {
        for (int i = 0; i < entities.size(); i++) {
            for (int j = i + 1; j < entities.size(); j++) {
                checkAndResolveCollision(entities[i], entities[j]);
            }
        }
    }

    void tryAddEntity(Entity* entity) override {
        if (entity->getComponent<BoxColliderComponent>()) {
            entities.push_back(entity);
        }
    }

private:
    void checkAndResolveCollision(Entity* entityA, Entity* entityB) {
        BoxColliderComponent* boxA = entityA->getComponent<BoxColliderComponent>();
        BoxColliderComponent* boxB = entityB->getComponent<BoxColliderComponent>();

        SDL_Rect rectA = boxA->getWorldSpaceRect();
        SDL_Rect rectB = boxB->getWorldSpaceRect();

        if (SDL_HasIntersection(&rectA, &rectB)) {
            // Assuming entityA is the player and entityB is the wall

            // Get the player's transform component
            TransformComponent* transformA = entityA->getComponent<TransformComponent>();

            // Calculate the depth of the intersection on both axes
            int xOverlap = std::min(rectA.x + rectA.w, rectB.x + rectB.w) - std::max(rectA.x, rectB.x);
            int yOverlap = std::min(rectA.y + rectA.h, rectB.y + rectB.h) - std::max(rectA.y, rectB.y);

            // Resolve collision by pushing the player out of the wall
            // The player is moved out of the collision along the axis of least overlap
            Vector2f t = transformA->getPosition();
            if (xOverlap < yOverlap) {
                // Move player left or right out of collision
                if (rectA.x < rectB.x) t.x -= xOverlap;  // Move left
                else t.x += xOverlap;  // Move right
            }
            else {
                // Move player up or down out of collision
                if (rectA.y < rectB.y) t.y -= yOverlap;  // Move up
                else t.y += yOverlap;  // Move down
            }
            transformA->setPosition(t);
        }
    }

};
