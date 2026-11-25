#pragma once
#include "Bug.h"
#include "Lasso.h"

constexpr float GAMETIMER = 60.0f;
constexpr int MAX_BUGS = 6;
constexpr float EFFECTTIMER = 0.2f;
constexpr float EFFECTSCALE = WINDOWSIZE * 0.0035;

enum class GameState
{
	START,
	PLAYING,
	FINISHED,
	PAUSED
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
	void handleInputPaused();

	//draw functions
	void drawStart();
	void drawPlaying();
	void drawFinished();
	void drawPaused();
	void drawTimerAndScore();

	//game related functions
	void updateGameState(float deltaTime);
	void updateBugs(float deltaTime);
	void populateBugs();
	bool isPointOnScreen(const glm::vec2 inPoint);
	void determineCaught();

	//Game State
	GameState gameState = GameState::START;
	std::vector<Bug> bugs{};
	float timer = GAMETIMER;
	uint32_t score = 0;
	bool mousePressed = false;
	Lasso lasso;

	//SDL Stuff
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event{};

	//Background / Menu Sprites
	StaticSprite bgSprite;
	StaticSprite startSprite;
	StaticSprite retrySprite;
	StaticSprite exitSprite;
	StaticSprite continueSprite;

	//+points effect
	int effectCount = 0;
	float effectTimer = 0.0f;
	glm::vec2 effectPos{ 0, 0 };
};