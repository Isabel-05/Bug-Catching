#pragma once
#include "Sprite.h"
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

//feel free to change to a resolution you find comfortable
const int WINDOWSIZE = 1080;

//scaled proportionally for other resolutions
const float SPRITESIZE = 64 * (WINDOWSIZE / 1080.0f); 
const float SPAWNMARGIN = 40 * (WINDOWSIZE / 1080.0f);

enum class BugType
{
	LADYBUG = 0,
	MOTH = 1,	
	BUTTERFLY = 2
};

struct Bug
{
	Bug() = default;
	~Bug() = default;

	//See Sprite.h and Sprite.cpp comments
	Bug(const Bug&) = delete;
	Bug& operator=(const Bug&) = delete;
	//default so it uses the sprite class's move operators
	Bug(Bug&&) = default;
	Bug& operator=(Bug&&) = default;


	void init(SDL_Renderer* renderer, BugType inType, glm::vec2 inPosition, glm::vec2 inForward);
	void initRandom(SDL_Renderer* renderer);

	void update(float deltaTime);
	void renderSprite(SDL_Renderer* renderer);

	BugType bugType = BugType::LADYBUG;
	glm::vec2 position = glm::vec2(0, 0);
	uint32_t pointsAwarded = 0; //how many points you get for catching the bug

private:

	void resetTimer();

	Sprite sprite;
	glm::vec2 forward{0, 0};
	int lifeTime = 5; //the number of direction changes before the bug leaves the screen
	float movementSpeed = 0.0f;
	float changeDirTimer = 0.0f;
};