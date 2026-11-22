#include "Sprite.h"

Sprite::~Sprite()
{
	SDL_DestroyTexture(texture);
}

//during move construction, the original object is about to be destroyed meaning a shallow copy would just give
//the new object a raw gpu pointer thats about to be freed from the SDL_DestroyTexture call in the destructor of the original obj
//so i move over the pointer to the new obj and then assign the obj thats about to be destroyed nullptr so it cant free it. Equals no dangling pointer
Sprite::Sprite(Sprite&& other) noexcept
{
	texture = other.texture;
	other.texture = nullptr;
}

// its the same for this one but since this isnt a constructor ie the object already 
// exists and has its own texture, its own texture still needs to be freed before it can take on a new one
// + safe guard for self move
Sprite& Sprite::operator=(Sprite&& other) noexcept
{
	if (this != &other)
	{
		SDL_DestroyTexture(texture);
		texture = other.texture;
		other.texture = nullptr;
	}
	return *this;
}

void Sprite::init(SDL_Renderer* renderer, const std::string& filename)
{
	SDL_Surface* imageSf = SDL_LoadBMP(filename.c_str());

	if (!imageSf)
	{
		SDL_Log("Failed to load image: %s (%s)", filename.c_str(), SDL_GetError());
		return;
	}

	texture = SDL_CreateTextureFromSurface(renderer, imageSf);

	if (!texture) {
		SDL_Log("Failed to create texture from: %s (%s)", filename.c_str(), SDL_GetError());
	}

	SDL_DestroySurface(imageSf);
}

void Sprite::render(SDL_Renderer* renderer, const SDL_FRect& inRect)
{
	if (!texture) { return; }
	
	SDL_RenderTexture(renderer, texture, nullptr, &inRect);
}

void Sprite::renderRotated(SDL_Renderer* renderer, const SDL_FRect& inRect, const double angle, bool flip)
{
	if (!texture) { return; }

	SDL_FPoint center = { inRect.w / 2, inRect.h / 2 };
	SDL_FlipMode flipmode = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	SDL_RenderTextureRotated(renderer, texture, nullptr, &inRect, angle, &center, flipmode);
}
