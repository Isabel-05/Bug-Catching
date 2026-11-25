#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "Sprite.h"
#include "glm/glm.hpp"


class Lasso
{
public:
	Lasso() = default;
	~Lasso() = default;
	Lasso(const Lasso&) = delete;
	Lasso& operator=(const Lasso&) = delete;
	Lasso(Lasso&&) = default;
	Lasso& operator=(Lasso&&) = default;

	void init(SDL_Renderer* renderer, float scalar);

	void drawLasso(SDL_Renderer* renderer);
	void update(float deltaTime);
	void addPoint(const SDL_FPoint& point);

	bool isPointInLasso(const glm::vec2 inPoint);
	bool isClosed();
	glm::vec2 getCenter();
	void clearPoints();
	void startFade();
	void startPop();

private:

	std::vector<SDL_FPoint> lassoPoints{};
	std::vector<SDL_Vertex> vertices{};
	std::vector<int> indices{};

	Sprite blueLine;
	Sprite redLine;
	Sprite greenLine;

	bool fading = false;
	float alpha = 1.0f;

	bool expanding = false;
	float uvScale = 0.0f;
};

