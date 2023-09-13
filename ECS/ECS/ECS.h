#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <typeindex>
#include <memory>
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>  
#include <set>
#include <utility>
#include <SDL.h>
#include <SDL_image.h>
#include "PCM.h"
#include "Camera.h"
#include "Renderer.h"


/*
TODO:
Fix delta time calulation when switching scenes
Change default windows bar
Tilemap
Sound
Light
Particles
*/

using namespace PC;

class Entity;
class Component;
class System;

class Entity : public std::enable_shared_from_this<Entity> {
public:
    std::string name = "";

private:
    inline static std::vector<std::shared_ptr<Entity>> allEntities = {};
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    Entity() {}

public:
    template <typename T, typename... TArgs>
    std::shared_ptr<T> addComponent(TArgs&&... args) {
        auto comp = std::make_shared<T>(std::forward<TArgs>(args)...);
        comp->owner = this->shared_from_this(); // Set the owner of the component to this entity
        components[std::type_index(typeid(T))] = comp;
        return comp;
    }

    template<typename T>
    std::shared_ptr<T> getComponent() {
        auto iter = components.find(typeid(T));
        if (iter != components.end()) {
            return std::dynamic_pointer_cast<T>(iter->second);
        }
        return nullptr;
    }

    static std::vector<std::shared_ptr<Entity>>& getAllEntities() {
        std::cout << allEntities.size() << std::endl;
        return allEntities;
    }

    static std::shared_ptr<Entity> create() {
        std::shared_ptr<Entity> entity(new Entity());
        allEntities.push_back(entity);
        return entity;
    }

    static void destroyAllEntities() {
        allEntities.clear();
    }
};

class Component {
public:
    std::weak_ptr<Entity> owner; // weak_ptr here to break the cyclic reference

    virtual ~Component() {}

    Component() = default;

    template <typename T>
    std::shared_ptr<T> getComponent() const {
        if (auto sharedOwner = owner.lock()) {
            return sharedOwner->getComponent<T>();
        }
        return nullptr;
    }
};

class System {
public:
    std::vector<std::weak_ptr<Entity>> entities; // weak_ptr here to ensure we don't unnecessarily keep entities alive

    virtual void tryAddEntity(std::shared_ptr<Entity> entity) = 0;
};

class SystemManager {
public:
    std::vector<std::shared_ptr<System>> systems;

    template<typename T, typename... TArgs>
    std::shared_ptr<T> registerSystem(TArgs&&... args) {
        auto t = std::make_shared<T>(std::forward<TArgs>(args)...);
        systems.emplace_back(t);
        return t;
    }

    void addAllEntitiesToSystems(std::vector<std::shared_ptr<Entity>>& allEntities) {
        for (auto& entity : allEntities) {
            addEntityToSystems(entity);
        }
    }

    void resetAllEntities() {
        for (auto& system : systems) {
            system->entities.clear();
        }
        Entity::destroyAllEntities();
    }

    void resetScene() {
        resetAllEntities();
    }

private:
    void addEntityToSystems(std::shared_ptr<Entity> entity) {
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
    float frameTime; // Changed from frameDelay (Uint32) to frameTime (float)
    SDL_RendererFlip flip;

    AnimationState(int beginFrameIndex = 0, int frameCount = 0, float frameTime = 0.5f, SDL_RendererFlip flip = SDL_FLIP_NONE)
        : beginFrameIndex(beginFrameIndex), frameCount(frameCount), frameTime(frameTime / 1000), flip(flip) {}
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
    SDL_RendererFlip flip;
    std::unordered_map<std::string, AnimationState> animationStates;
    std::string currentState;
    bool animationPlaying;
    float frameTime;
    float elapsedTime; 

    SpriteComponent(const char* path, int tolerance = 12,
        int yThreshold = 10)
        : startingFrame(0), currentFrame(0), lastFrameTime(0),
        frameTime(60), flip(SDL_FLIP_NONE), elapsedTime(0.0f), animationPlaying(false) {

        spriteSheet = IMG_LoadTexture(Renderer::Instance().Get(), path);
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
            flip = state.flip; 
            frameTime = state.frameTime;
        }
        else {
            std::cerr << "No such animation state exists: " << stateName << std::endl;
        }
    }

    void nextFrame(float deltaTime) {
        // Increment the elapsed time by delta time.
        elapsedTime += deltaTime;

        // If enough time has passed since the last frame...
        if (elapsedTime >= frameTime) {
            // Reset the elapsed time
            elapsedTime -= frameTime;

            // Move to the next frame
            currentFrame = (currentFrame - startingFrame + 1) % frameCount + startingFrame;

            srcRect = frames[currentFrame];

            auto it = animationStates.find(currentState);
            if (it != animationStates.end()) {
                AnimationState& state = it->second;
                // Check if we've reached the end of the animation.
                if (currentFrame - startingFrame + 1 >= state.frameCount) {
                    animationPlaying = false;
                    // Reset to the first frame of the animation.
                    currentFrame = state.beginFrameIndex;
                }
            }
        }
    }

    SDL_Rect getWorldSpaceRect() {
        auto transform = owner.lock()->getComponent<TransformComponent>();
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
    SDL_Texture* texture;

    SquareComponent(Rectangle rect, SDL_Color color)
        : rect(rect), color(color) {

        SDL_Renderer* renderer = Renderer::Instance().Get();

        // Create the main rectangle texture.
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, rect.width, rect.height);

        // Set the texture as the render target.
        SDL_SetRenderTarget(renderer, texture);

        // Draw the main rectangle.
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect mainRect = { 0, 0, rect.width, rect.height };
        SDL_RenderFillRect(renderer, &mainRect);

        // Reset the render target.
        SDL_SetRenderTarget(renderer, NULL);
    }

    ~SquareComponent() {
        SDL_DestroyTexture(texture);
    }

    SDL_Rect getWorldSpaceRect() {
        auto transform = owner.lock()->getComponent<TransformComponent>();
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

struct OBB {
    Vector2f center;
    Vector2f extents;
    Matrix3x3f rotationMatrix;

    OBB() = default;
    OBB(const Vector2f& center, const Vector2f& extents, const Matrix3x3f& rotationMatrix)
        : center(center), extents(extents), rotationMatrix(rotationMatrix) { }

    void projectOntoAxis(const Vector2f& axis, float& min, float& max) const {
        Vector2f vertices[4] = {
            center + (rotationMatrix * (extents * Vector2f(1, 1))),
            center + (rotationMatrix * (extents * Vector2f(1, -1))),
            center + (rotationMatrix * (extents * Vector2f(-1, -1))),
            center + (rotationMatrix * (extents * Vector2f(-1, 1)))
        };

        min = max = axis.dotProduct(vertices[0]);

        for (int i = 1; i < 4; i++) {
            float projection = axis.dotProduct(vertices[i]);

            if (projection < min) min = projection;
            else if (projection > max) max = projection;
        }
    }

    std::vector<Vector2f> getAxes() const {
        // Retrieve the corners (vertices) of the OBB
        Vector2f vertices[4] = {
            center + (rotationMatrix * (extents * Vector2f(1, 1))),
            center + (rotationMatrix * (extents * Vector2f(1, -1))),
            center + (rotationMatrix * (extents * Vector2f(-1, -1))),
            center + (rotationMatrix * (extents * Vector2f(-1, 1)))
        };

        // Compute the edge vectors from the vertices
        Vector2f edges[2] = {
            vertices[1] - vertices[0],  // Edge from vertex 0 to 1
            vertices[3] - vertices[0]   // Edge from vertex 0 to 3
        };

        // Find normals (axes) to the edge vectors
        std::vector<Vector2f> axes;
        for (int i = 0; i < 2; i++) {
            Vector2f normal(-edges[i].y, edges[i].x);
            axes.push_back(normal.normalize());
        }

        return axes;
    }

};

enum CollisionLayer {
    Default = 0,
    LayerOne,
    LayerTwo,
    LayerThree,
    LayerFour,
    LayerFive,
    LayerSix,
    LayerSeven,
    LayerEight,
    LayerNine,
    LastLayer,
};

class CollisionMatrix {
public:
    CollisionMatrix() {
        // By default, all layers collide with each other
        for (int i = 0; i < LastLayer; i++) {
            for (int j = 0; j < LastLayer; j++) {
                matrix[i][j] = true;
            }
        }
    }

    // Set if two layers should collide
    void setShouldCollide(CollisionLayer layer1, CollisionLayer layer2, bool shouldCollide) {
        matrix[layer1][layer2] = shouldCollide;
        matrix[layer2][layer1] = shouldCollide; // Ensure symmetry
    }

    // Check if two layers should collide
    bool shouldCollide(CollisionLayer layer1, CollisionLayer layer2) const {
        return matrix[layer1][layer2];
    }

private:
    bool matrix[LastLayer][LastLayer];
};

class BoxColliderComponent : public Component {
public:
    BoxColliderComponent() : customCollider(false) { }

    BoxColliderComponent(Rectangle rect)
        : rect(rect), customCollider(true) { }


    SDL_Rect getWorldSpaceRect() {
        auto sprite = owner.lock()->getComponent<SpriteComponent>();
        auto square = owner.lock()->getComponent<SquareComponent>();

        if (customCollider) {            
            auto transform = owner.lock()->getComponent<TransformComponent>();
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

    OBB getWorldSpaceOBB() {
        SDL_Rect temp = getWorldSpaceRect();
        auto transform = owner.lock()->getComponent<TransformComponent>();

        // Calculate the center of the rectangle
        Vector2f center(static_cast<float>(temp.x + temp.w / 2), static_cast<float>(temp.y + temp.h / 2));

        return OBB(center,
            Vector2f(static_cast<float>(temp.w) / 2, static_cast<float>(temp.h) / 2),
            Matrix3x3f::Matrix3x3FromRotation(transform->getRotation()));
    }

    void setLayer(CollisionLayer layer) {
        this->layer = layer;
    }

    CollisionLayer getLayer() const {
        return layer;
    }
private:
    Rectangle rect;
    bool customCollider;
    std::vector<std::function<void(Entity*, Entity*)>> collisionHandlers;
    CollisionLayer layer = Default;
};

class PhysicsComponent : public Component {
public:
    Vector2f velocity;
    Vector2f acceleration;
    float mass;
    float inverseMass;   // added this
    float restitution;   // added this
    bool isStatic;
    bool isAffectedByGravity;
    bool isGrounded;
    float damping = 0.99f;

    PhysicsComponent(float mass, bool isAffectedByGravity = true, bool isStatic = false, float restitution = 0.5f)
        : mass(mass), restitution(restitution), isAffectedByGravity(isAffectedByGravity), isStatic(isStatic),
        velocity(0, 0), acceleration(0, 0), isGrounded(false)
    {
        if (isStatic) {
            isAffectedByGravity = false;
            mass = std::numeric_limits<float>::infinity();
            inverseMass = 0;   // for static objects, inverse mass is zero
        }
        else {
            inverseMass = 1 / mass;  // calculate inverse mass for dynamic objects
        }
    }

    void applyForce(const Vector2f& force) {
        if (!isStatic) {
            acceleration += force / mass;
            isGrounded = false;
        }
    }
};

class RenderLayerComponent : public Component {
public:
    int layer;

    RenderLayerComponent(int layer) : layer(layer) {}
};

class Script {
public:
    std::weak_ptr<Entity> entity; // Using weak_ptr to break cyclic reference

    virtual void start() { }

    virtual void update(float deltaTime) { }

    virtual void onCollision(const std::shared_ptr<Entity>& other) { }

    virtual void onCollisionExit(const std::shared_ptr<Entity>& other) { }

    void setEntity(const std::shared_ptr<Entity>& entityPtr) {
        this->entity = entityPtr;
    }
};

class ScriptComponent : public Component {
public:
    void addScript(std::shared_ptr<Script> script) {
        script->setEntity(owner.lock()); // Convert weak_ptr to shared_ptr
        scripts.push_back(script);
    }

    void start() {
        for (auto& script : scripts) {
            script->start();
        }
    }

    void update(float deltaTime) {
        for (auto& script : scripts) {
            script->update(deltaTime);
        }
    }

    void onCollision(const std::shared_ptr<Entity>& other) {
        for (auto& script : scripts) {
            script->onCollision(other);
        }
    }

    void onCollisionExit(const std::shared_ptr<Entity>& other) {
        for (auto& script : scripts) {
            script->onCollisionExit(other);
        }
    }
private:
    std::vector<std::shared_ptr<Script>> scripts;
};

class RenderSystem : public System {
public:
    SDL_Renderer* renderer;
    std::map<int, std::vector<std::shared_ptr<Entity>>> renderLayers;

    RenderSystem(bool showColliders = false, int ssaaFactor = 2) :
        renderer(Renderer::Instance().Get()),
        showColliders(showColliders), ssaaFactor(ssaaFactor)
    {
    #ifdef NDEBUG
        showColliders = false;
    #endif

        int windowWidth, windowHeight;
        SDL_GetWindowSize(Renderer::Instance().GetWindow(), &windowWidth, &windowHeight);

        // Create the supersampled texture
        ssaaTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowWidth * ssaaFactor, windowHeight * ssaaFactor);
    }

    void update(float deltaTime) {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(Renderer::Instance().GetWindow(), &windowWidth, &windowHeight);
        SDL_SetRenderTarget(renderer, ssaaTexture);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        for (auto& layer : renderLayers) {
            for (const auto& entity : layer.second) {
                auto transform = entity->getComponent<TransformComponent>();
                auto sprite = entity->getComponent<SpriteComponent>();
                auto shape = entity->getComponent<SquareComponent>();
                if (sprite) {
                    SDL_Rect temp = sprite->getWorldSpaceRect();

                    float rot = transform->getWorldSpaceRotation();

                    SDL_RenderCopyEx(renderer, sprite->spriteSheet, &(sprite->srcRect), &temp,
                        transform->getRotation(), nullptr, sprite->flip);
                    sprite->nextFrame(deltaTime);
                #ifdef _DEBUG
                    auto boxCollider = entity->getComponent<BoxColliderComponent>();
                    if (boxCollider && showColliders) {
                        SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);  // Light blue

                        SDL_Point points[5];
                        computeRotatedBox(boxCollider, rot, points);
                        SDL_RenderDrawLines(renderer, points, 5);                
                    }
                #endif
                }
                else if (shape) {
                    SDL_Rect temp = shape->getWorldSpaceRect();
                    float rot = transform->getWorldSpaceRotation();

                    SDL_RenderCopyEx(renderer, shape->texture, NULL, &temp, rot, NULL,
                        SDL_FLIP_NONE);
                #ifdef _DEBUG
                    auto boxCollider = 
                        entity->getComponent<BoxColliderComponent>();

                    if (boxCollider && showColliders) {
                        SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);  // Light blue

                        SDL_Point points[5];
                        computeRotatedBox(boxCollider, rot, points);
                        SDL_RenderDrawLines(renderer, points, 5);
                    }
                #endif                
                }
            }
            
        }

        // Now, reset to the default render target
        SDL_SetRenderTarget(renderer, NULL);

        // Draw the supersampled texture to the default render target
        SDL_Rect srcRect = { 0, 0, windowWidth * ssaaFactor, windowHeight * ssaaFactor };
        SDL_Rect dstRect = { 0, 0, windowWidth, windowHeight };
        SDL_RenderCopy(renderer, ssaaTexture, &srcRect, &dstRect);

        // Present the final rendering to the window
        SDL_RenderPresent(renderer);
    }

    void tryAddEntity(std::shared_ptr<Entity> entity) override { // Use shared_ptr
        if (entity->getComponent<TransformComponent>() && (entity->getComponent<SpriteComponent>() || entity->getComponent<SquareComponent>())) {
            auto layerComp = entity->getComponent<RenderLayerComponent>();
            if (layerComp) {
                renderLayers[layerComp->layer].push_back(entity);
            }
            else {
                renderLayers[0].push_back(entity);
            }
        }
    }
private:
    SDL_Texture* ssaaTexture;
    int ssaaFactor;
    bool showColliders;
private:
#ifdef _DEBUG
    void computeRotatedBox(std::shared_ptr<BoxColliderComponent> box, float rotation, SDL_Point points[5]) {
        SDL_Rect rect = box->getWorldSpaceRect();
        int x = rect.x;
        int y = rect.y;
        int w = rect.w;
        int h = rect.h;

        double rad = rotation * M_PI / 180;

        // Find the center of the box
        SDL_Point center = { x + w / 2, y + h / 2 };

        // Compute each corner of the box relative to the center, then rotate
        points[0].x = static_cast<int>(std::round(center.x + (x - center.x) * cos(rad) - (y - center.y) * sin(rad)));
        points[0].y = static_cast<int>(std::round(center.y + (x - center.x) * sin(rad) + (y - center.y) * cos(rad)));

        points[1].x = static_cast<int>(std::round(center.x + ((x + w) - center.x) * cos(rad) - (y - center.y) * sin(rad)));
        points[1].y = static_cast<int>(std::round(center.y + ((x + w) - center.x) * sin(rad) + (y - center.y) * cos(rad)));

        points[2].x = static_cast<int>(std::round(center.x + ((x + w) - center.x) * cos(rad) - ((y + h) - center.y) * sin(rad)));
        points[2].y = static_cast<int>(std::round(center.y + ((x + w) - center.x) * sin(rad) + ((y + h) - center.y) * cos(rad)));

        points[3].x = static_cast<int>(std::round(center.x + (x - center.x) * cos(rad) - ((y + h) - center.y) * sin(rad)));
        points[3].y = static_cast<int>(std::round(center.y + (x - center.x) * sin(rad) + ((y + h) - center.y) * cos(rad)));

        points[4] = points[0];  // Close the loop
    }
#endif
};

class InputSystem {
public:
    static InputSystem& getInstance() {
        static InputSystem instance;
        return instance;
    }

    static bool isKeyDown(SDL_Keycode key) {
        return getInstance().keyStates.find(key) != getInstance().keyStates.end() &&
            getInstance().keyStates.at(key);
    }

    static bool isKeyUp(SDL_Keycode key) {
        return getInstance().keyStates.find(key) == getInstance().keyStates.end() ||
            !getInstance().keyStates.at(key);
    }

    static bool isMouseButtonDown(uint8_t button) {
        return getInstance().mouseButtonStates.find(button) != getInstance().mouseButtonStates.end() &&
            getInstance().mouseButtonStates.at(button);
    }

    static bool isMouseButtonUp(uint8_t button) {
        return getInstance().mouseButtonStates.find(button) == getInstance().mouseButtonStates.end() ||
            !getInstance().mouseButtonStates.at(button);
    }

    void update(SDL_Event& event) {
        // Update the key states based on the event
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            SDL_Keycode key = event.key.keysym.sym;
            keyStates[key] = (event.type == SDL_KEYDOWN);
        }

        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            uint8_t button = event.button.button;
            mouseButtonStates[button] = (event.type == SDL_MOUSEBUTTONDOWN);
        }
    }

private:
    InputSystem() = default; // Private constructor
    InputSystem(const InputSystem&) = delete; // Prevent copy
    void operator=(const InputSystem&) = delete; // Prevent assignment

    std::unordered_map<SDL_Keycode, bool> keyStates;
    std::unordered_map<uint8_t, bool> mouseButtonStates;
};

class WorldSpaceSystem : public System {
public:
    std::shared_ptr<Camera> cam;

    WorldSpaceSystem(std::shared_ptr<Camera> cam) : cam(cam) {}

    void update() {
        for (const auto& entity : entities) {
            auto transform = entity.lock()->getComponent<TransformComponent>();
            if (transform) {
                Matrix3x3<float> worldSpace = cam->getTransformMatrix() *
                    transform->getTransformMatrix();
                transform->setWorldSpaceMatrix(worldSpace);
            }
        }
    }

    void tryAddEntity(std::shared_ptr<Entity> entity) override {
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
                OBBCollision(entities[i], entities[j]);
            }
        }
    }

    void tryAddEntity(std::shared_ptr<Entity> entity) override {
        if (entity->getComponent<BoxColliderComponent>() && entity->getComponent<PhysicsComponent>()) {
            entities.push_back(entity);
        }
    }
private:
    std::pair<bool, Vector2f> checkOBBCollisionAndGetMTV(const OBB& obbA, const OBB& obbB) {
        Vector2f mtv; // Minimum Translation Vector
        float minOverlap = std::numeric_limits<float>::max();

        // Compute the axes for SAT
        std::vector<Vector2f> axes = obbA.getAxes();
        std::vector<Vector2f> axesB = obbB.getAxes();
        axes.insert(axes.end(), axesB.begin(), axesB.end());

        // Check overlap along each axis
        for (Vector2f& axis : axes) {
            float minA, maxA, minB, maxB;

            obbA.projectOntoAxis(axis, minA, maxA);
            obbB.projectOntoAxis(axis, minB, maxB);

            float overlap = std::min(maxA, maxB) - std::max(minA, minB);

            // If no overlap found on any axis, no collision
            if (overlap <= 0) {
                return { false, {} };
            }
            else {
                if (overlap < minOverlap) {
                    minOverlap = overlap;
                    if (maxB - minA < maxA - minB) {
                        mtv = -axis * overlap;
                    }
                    else {
                        mtv = axis * overlap;
                    }
                }
            }
        }

        return { true, mtv };
    }

    void resolveAndRespondToCollision(std::weak_ptr<Entity> entityA, std::weak_ptr<Entity> entityB, OBB obbA, OBB obbB, Vector2f& mtv) {
        auto transformA = entityA.lock()->getComponent<TransformComponent>();
        auto transformB = entityB.lock()->getComponent<TransformComponent>();
        auto physicsA = entityA.lock()->getComponent<PhysicsComponent>();
        auto physicsB = entityB.lock()->getComponent<PhysicsComponent>();
        if (abs(mtv.x) < 0.01f && mtv.y > 0 && physicsA->velocity.y > 0) {
            physicsA->velocity.y = 0;  // Reset the downward velocity
            physicsA->isGrounded = true;
        }

        float minOverlap = std::numeric_limits<float>::max();

        // Compute the axes for SAT
        std::vector<Vector2f> axes = obbA.getAxes();
        std::vector<Vector2f> axesB = obbB.getAxes();
        axes.insert(axes.end(), axesB.begin(), axesB.end());

        // Check overlap along each axis
        for (Vector2f& axis : axes) {
            float minA, maxA, minB, maxB;

            obbA.projectOntoAxis(axis, minA, maxA);
            obbB.projectOntoAxis(axis, minB, maxB);

            float overlap = std::min(maxA, maxB) - std::max(minA, minB);

            // If no overlap found on any axis, no collision
            if (overlap <= 0) {
                return;
            }
            else {
                if (overlap < minOverlap) {
                    minOverlap = overlap;
                    if (maxB - minA < maxA - minB) {
                        mtv = -axis * overlap;
                    }
                    else {
                        mtv = axis * overlap;
                    }
                }
            }
        }

        // Translate entities by the MTV to resolve the collision
        Vector2f posA = transformA->getPosition();
        Vector2f posB = transformB->getPosition();

        //std::cout << mtv << std::endl;
        if (!physicsA->isStatic && !physicsB->isStatic) {
            posA -= mtv * 0.5f;
            posB += mtv * 0.5f;
        }
        else if (!physicsA->isStatic) {
            posA -= mtv;
        }
        else if (!physicsB->isStatic) {
            posB += mtv;
        }

        transformA->setPosition(posA);
        transformB->setPosition(posB);

        // Collision response (adjust velocities, etc.)
        // This can be expanded further for more realistic response.
        if (!physicsA->isStatic && !physicsB->isStatic) {
            float totalMass = physicsA->mass + physicsB->mass;
            Vector2f velocityA = physicsA->velocity;
            Vector2f velocityB = physicsB->velocity;

            physicsA->velocity = velocityA - 2 * physicsB->mass / totalMass * (velocityA - velocityB);
            physicsB->velocity = velocityB - 2 * physicsA->mass / totalMass * (velocityB - velocityA);

            physicsA->isGrounded = false;
            physicsB->isGrounded = false;
        }
    }

    void OBBCollision(std::weak_ptr<Entity> entityA, std::weak_ptr<Entity> entityB) {
        auto boxA = entityA.lock()->getComponent<BoxColliderComponent>();
        auto boxB = entityB.lock()->getComponent<BoxColliderComponent>();

        auto scriptA = entityA.lock()->getComponent<ScriptComponent>();
        auto scriptB = entityB.lock()->getComponent<ScriptComponent>();

        OBB obbA = boxA->getWorldSpaceOBB();
        OBB obbB = boxB->getWorldSpaceOBB();

        auto [isOverlapping, mtv] = checkOBBCollisionAndGetMTV(obbA, obbB);
        if (isOverlapping) {
            if (collisionMatrix.shouldCollide(boxA->getLayer(), boxB->getLayer())) {
                resolveAndRespondToCollision(entityA, entityB, obbA, obbB, mtv); 
            }

            if (scriptA) {
                scriptA->onCollision(entityB.lock());
            }
            if (scriptB) {
                scriptB->onCollision(entityA.lock());
            }

            currentCollisions.insert({ entityA.lock().get(), entityB.lock().get() });
        }
        else {
            if (currentCollisions.count({ entityA.lock().get(), entityB.lock().get() }) > 0) {
                currentCollisions.erase({ entityA.lock().get(), entityB.lock().get() });
                if (scriptA) {
                    scriptA->onCollisionExit(entityB.lock());
                }
                if (scriptB) {
                    scriptB->onCollisionExit(entityA.lock());
                }
            }
        }
    }

public:
    CollisionMatrix collisionMatrix;
private:
    std::set<std::pair<Entity*, Entity*>> currentCollisions;
    
};

class PhysicsSystem : public System {
public:
    const Vector2f gravity = Vector2f(0, 9.8f);

    void update(float deltaTime) {
        for (const auto& entity : entities) {
            auto physics = entity.lock()->getComponent<PhysicsComponent>();
            if (physics && !physics->isStatic) {
                if (physics->isAffectedByGravity && !physics->isGrounded) {
                    // Apply gravity
                    physics->applyForce(gravity * physics->mass);
                }

                // Update velocity based on acceleration
                physics->velocity += physics->acceleration;

                // Apply damping to the velocity
                physics->velocity *= physics->damping;

                // Update position based on velocity
                auto transform = entity.lock()->getComponent<TransformComponent>();
                if (transform) {
                    Vector2f pos = transform->getPosition();
                    pos += physics->velocity * deltaTime;
                    transform->setPosition(pos);
                }

                // Reset acceleration for next frame
                physics->acceleration = Vector2f(0, 0);
            }
        }
    }

    void tryAddEntity(std::shared_ptr<Entity> entity) override {
        if (entity->getComponent<PhysicsComponent>()) {
            entities.push_back(entity);
        }
    }
};

class ScriptSystem : public System {
public:
    void start() {
        for (const auto& entity : entities) {
            auto script = entity.lock()->getComponent<ScriptComponent>();
            if (script) {
                script->start();
            }
        }
    }

    void update(float deltaTime) {
        for (const auto& entity : entities) {
            auto script = entity.lock()->getComponent<ScriptComponent>();
            if (script) {
                script->update(deltaTime);
            }
        }
    }

    void tryAddEntity(std::shared_ptr<Entity> entity) override {
        if (entity->getComponent<ScriptComponent>()) {
            entities.push_back(entity);
        }
    }
};

