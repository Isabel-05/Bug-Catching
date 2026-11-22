#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <memory>

struct Sprite
{
	Sprite() = default;
	~Sprite();

	//I chose to write custom move assignment/constructor operators and delete the copy instead of a custom deep copy
	//since the only time bugs get moved/copied is when the vector overflows and has to realocate
	//in which case a move constructor suffices or when an object gets erased which just moves all elements down
	//to the erased elements spot via move assignment, so a copy operator is never really needed and move is easier

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;

	Sprite(Sprite&& other) noexcept;
	Sprite& operator=(Sprite&& other) noexcept;

	void init(SDL_Renderer* renderer, const std::string& filename);
	void render(SDL_Renderer* renderer, const SDL_FRect& inRect);
	void renderRotated(SDL_Renderer* renderer, const SDL_FRect& inRect, const double angle, bool flip);

	SDL_Texture* texture = nullptr;
};

struct StaticSprite
{
	SDL_FRect rect{};
	Sprite sprite;
};