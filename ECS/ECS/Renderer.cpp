#include "Renderer.h"

Renderer& Renderer::Instance()
{
	static Renderer instance;
	return instance;
}

SDL_Renderer* Renderer::Get() const
{
	return renderer.get();
}

void Renderer::Initialize(SDL_Window* window)
{
	renderer.reset(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
}

Renderer::Renderer() : renderer(nullptr, SDL_DestroyRenderer) {}

Renderer::~Renderer() {}
