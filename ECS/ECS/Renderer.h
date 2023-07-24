#pragma once
#include <SDL.h>
#include <memory>

class Renderer {
public:
    static Renderer& Instance();
    
    SDL_Renderer* Get() const;
    
    void Initialize(SDL_Window* window);

private:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
};
