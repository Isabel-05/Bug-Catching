#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "glm/glm.hpp"


class Lasso
{
public:
	Lasso() = default;
	~Lasso();
	void init(SDL_Renderer* renderer);

	bool isPointInLasso(const glm::vec2 inPoint);
	bool isClosed();
	void drawLasso(SDL_Renderer* renderer);

	void addPoint(SDL_FPoint& point);
	void clearPoints();

private:

	std::vector<SDL_FPoint> lassoPoints{};
	std::vector<SDL_Vertex> vertices{};
	std::vector<int> indices{};

	SDL_Texture* lineTexture = nullptr;
};

