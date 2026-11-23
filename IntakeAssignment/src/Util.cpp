#include "Util.h"

namespace Util
{
	bool pointsWithinRadius(const SDL_FPoint& p1, const SDL_FPoint& p2, const float radius)
	{
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		float distSq = dx * dx + dy * dy;

		return distSq <= radius * radius;
	}

	bool isPointInRect(const SDL_FRect& rect, const SDL_FPoint& point)
	{
		return (point.x >= rect.x &&
			point.x <= rect.x + rect.w &&
			point.y >= rect.y &&
			point.y <= rect.y + rect.h);
	}

	//algorithm source: https://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/ 
	bool ccw(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& c)
	{
		return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
	}
}