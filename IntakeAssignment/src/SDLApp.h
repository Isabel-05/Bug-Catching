#pragma once
#include "Bug.h"
#include <vector>

const float GAMETIMER = 60.0f;
const float LASSOLENIENCY = WINDOWSIZE * 0.12;
const int MAX_BUGS = 6;

enum class GameState
{
	START,
	PLAYING,
	FINISHED
};


class SDLApp
{
public:
	SDLApp();
	~SDLApp();

	void update(float deltaTime);

	bool wantQuit = false;

private:

	//input handling
	void handleInputStart();
	void handleInputPlaying();
	void handleInputFinished();

	//draw functions
	void drawStart();
	void drawPlaying();
	void drawFinished();
	void drawText();

	//game related functions
	void updateGameState(float deltaTime);
	void updateBugs(float deltaTime);
	void populateBugs();
	void determineCaught();

	//helper/math functions
	bool pointsWithinRadius(const SDL_FPoint& p1, const SDL_FPoint& p2, const float radius);
	bool isPointInLasso(const std::vector<SDL_FPoint>& points, const glm::vec2 inPoint);
	bool isPointOnScreen(const glm::vec2 inPoint);
	bool isPointInRect(const SDL_FRect& rect, const glm::vec2& point);
	

	//Game State
	GameState gameState = GameState::START;
	std::vector<Bug> bugs;
	float timer = GAMETIMER;
	uint32_t score = 0;

	//SDL Stuff
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event;

	//input tracking
	bool mousePressed = false;
	std::vector<SDL_FPoint> lassoPoints;

	//Background / Menu Sprites
	StaticSprite bgSprite;
	StaticSprite startSprite;
	StaticSprite retrySprite;
	StaticSprite exitSprite;
};

