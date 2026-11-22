#pragma once
#include <SDL3/SDL.h>

namespace Util
{
	bool pointsWithinRadius(const SDL_FPoint& p1, const SDL_FPoint& p2, const float radius);
	bool isPointInRect(const SDL_FRect& rect, const SDL_FPoint& point);
}