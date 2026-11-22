#pragma once
#include "Bug.h"
#include "Lasso.h"

constexpr float GAMETIMER = 60.0f;
constexpr float LASSOLENIENCY = WINDOWSIZE * 0.12;
constexpr int MAX_BUGS = 6;

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

	//helper function
	bool isPointOnScreen(const glm::vec2 inPoint);

	//Game State
	GameState gameState = GameState::START;
	std::vector<Bug> bugs{};
	float timer = GAMETIMER;
	uint32_t score = 0;
	Lasso lasso;

	//SDL Stuff
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event{};

	//input tracking
	bool mousePressed = false;

	//Background / Menu Sprites
	StaticSprite bgSprite;
	StaticSprite startSprite;
	StaticSprite retrySprite;
	StaticSprite exitSprite;
};

