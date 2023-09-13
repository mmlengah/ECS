#include "Scene.h"
#include <iostream>

void SceneManager::SwitchScene(const std::string& sceneName) {
    if (currentScene) {
        currentScene->Unload();
    }

    auto it = scenes.find(sceneName);
    if (it == scenes.end()) {
        std::cerr << "Scene " << sceneName << " not found!" << std::endl;
        return;
    }

    currentScene = it->second;
    currentScene->Initialize();
    currentScene->Load();
    currentScene->RegisterEntities();
}

void SceneManager::AddScene(std::shared_ptr<Scene> scene) {
    scenes[scene->GetName()] = scene;
}

void SceneManager::Run()
{
    if (currentScene) {
        currentScene->Run();
    }
}

std::string SceneManager::GetCurrentScene()
{
    if (currentScene) {
        return currentScene->GetName();
    }
    return "";
}

Scene::Scene(const std::string& name, std::shared_ptr<Camera> cam) : sceneName(name), cam(std::make_shared<Camera>(*cam)),
quitManager(QuitManager::getInstance()), oldTime(0), currentTime(0), deltaTime(0.0f), systemManager(nullptr), renderSystem(nullptr),
worldSpaceSystem(nullptr), collisionSystem(nullptr), physicsSystem(nullptr), scriptSystem(nullptr), cameraTarget(nullptr)
{
}

void Scene::Initialize()
{
    systemManager = std::make_unique<SystemManager>();

    renderSystem = systemManager->registerSystem<RenderSystem>(true);
    worldSpaceSystem = systemManager->registerSystem<WorldSpaceSystem>(cam);
    collisionSystem = systemManager->registerSystem<CollisionSystem>();
    physicsSystem = systemManager->registerSystem<PhysicsSystem>();
    scriptSystem = systemManager->registerSystem<ScriptSystem>();
}


void Scene::SetCameraTarget(std::shared_ptr<Entity> target)
{
    cameraTarget = target;
}

void Scene::RegisterEntities()
{
    systemManager->addAllEntitiesToSystems(Entity::getAllEntities());
}

void Scene::Unload()
{
    Entity::destroyAllEntities();
}

void Scene::Update()
{
    if (cameraTarget) {
        cam->lookAt(cameraTarget->getComponent<TransformComponent>()->getPosition());
    }
    worldSpaceSystem->update();
    scriptSystem->update(deltaTime);
    collisionSystem->update();
    physicsSystem->update(deltaTime);
}

void Scene::Render()
{
    renderSystem->update(deltaTime);
}

void Scene::Run()
{
    //Initialize();
    //Load();
    //RegisterEntities();

    SDL_Event event;

    scriptSystem->start();
    while (sceneName == SceneManager::GetCurrentScene()) {

        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - oldTime) / 1000.0f; // convert from milliseconds to seconds
        std::cout << GetName() << " " << deltaTime << std::endl;
        oldTime = currentTime;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quitManager.setQuit(true);
                return;
            }
            InputSystem::getInstance().update(event);
        }

        Update();
        Render();
    }
}

void Scene::setShouldCollide(CollisionLayer layer1, CollisionLayer layer2, bool shouldCollide)
{
    collisionSystem->collisionMatrix.setShouldCollide(layer1, layer2, shouldCollide);
}

