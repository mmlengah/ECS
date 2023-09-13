#pragma once
#include <string>
#include <memory>
#include <map>
#include "QuitManager.h"
#include "ECS.h"

class Scene {
public:
    Scene(const std::string& name, std::shared_ptr<Camera> cam);

    virtual ~Scene() {}

    void Initialize();  
    virtual void Load() = 0;
    void SetCameraTarget(std::shared_ptr<Entity> target);
    void RegisterEntities();
    void Unload();     

    void Update();
    void Render();
    void Run();

    void setShouldCollide(CollisionLayer layer1, CollisionLayer layer2, bool shouldCollide);
 
    std::string GetName() const { return sceneName; }
private:
    std::string sceneName;

    QuitManager& quitManager;
    Uint32 oldTime, currentTime;
    float deltaTime;

    std::unique_ptr<SystemManager> systemManager;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<WorldSpaceSystem> worldSpaceSystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<PhysicsSystem> physicsSystem;
    std::shared_ptr<ScriptSystem> scriptSystem;
    std::shared_ptr<Camera> cam;
    std::shared_ptr<Entity> cameraTarget;
};

class SceneManager {
public:
    static void SwitchScene(const std::string& sceneName);
    static void AddScene(std::shared_ptr<Scene> scene);
    static void Run();
    static std::string GetCurrentScene();
private:
    inline static std::map<std::string, std::shared_ptr<Scene>> scenes;
    inline static std::shared_ptr<Scene> currentScene;
};

