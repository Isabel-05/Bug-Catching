#pragma once
#include <SDL3/SDL.h>
#include <string>

namespace Util
{
	bool pointsWithinRadius(const SDL_FPoint& p1, const SDL_FPoint& p2, const float radius);
	bool isPointInRect(const SDL_FRect& rect, const SDL_FPoint& point);
	bool ccw(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& c);

	SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filename);
}