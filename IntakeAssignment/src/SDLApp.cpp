#include "SDLApp.h"
#include "Util.h"

SDLApp::SDLApp()
{
	//init SDL
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Failed to initialize SDL");
		wantQuit = true;
		return;
	}
	
	//Init Window & Renderer
	SDL_CreateWindowAndRenderer("Bug Catching", WINDOWSIZE, WINDOWSIZE, 0, &window, &renderer);
	if (!window || !renderer)
	{
		SDL_Log("failed to allocate SDL resources");
		wantQuit = true;
		return;
	}

	SDL_SetRenderVSync(renderer, 1);

	lasso.init(renderer);

	//init Background / Menu sprites
	bgSprite.rect = { 0, 0, WINDOWSIZE, WINDOWSIZE };
	bgSprite.sprite.init(renderer, "./Images/Grass.bmp");

	float midScreen = WINDOWSIZE / 2;

	startSprite.rect = { midScreen - (midScreen / 2), midScreen - (midScreen / 4), WINDOWSIZE / 2, WINDOWSIZE / 4};
	startSprite.sprite.init(renderer, "./Images/start_box.bmp");

	retrySprite.rect = { midScreen - (midScreen / 2), midScreen - (midScreen / 4), WINDOWSIZE / 2, WINDOWSIZE / 4 };
	retrySprite.sprite.init(renderer, "./Images/retry_box.bmp");

	exitSprite.rect = { midScreen - (midScreen / 4), midScreen + (midScreen / 3), WINDOWSIZE / 4, WINDOWSIZE / 8 };
	exitSprite.sprite.init(renderer, "./Images/exit_box.bmp");
}

SDLApp::~SDLApp()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void SDLApp::update(float deltaTime)
{
	switch (gameState)
	{
	case GameState::START:
		handleInputStart();
		drawStart();
		break;
	case GameState::PLAYING:
		handleInputPlaying();
		updateGameState(deltaTime);
		drawPlaying();
		break;
	case GameState::FINISHED:
		handleInputFinished();
		drawFinished();
		break;
	}
}


//INPUT FUNCTIONS	

void SDLApp::handleInputStart()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) { wantQuit = true; }

		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {

			float x = event.button.x;
			float y = event.button.y;

			//start button clicked
			if (Util::isPointInRect(startSprite.rect, {x, y})) {
				gameState = GameState::PLAYING;
			}
			//exit button clicked
			else if (Util::isPointInRect(exitSprite.rect, {x, y})) {
				wantQuit = true;
			}
		}
	}
}

void SDLApp::handleInputPlaying()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) { wantQuit = true; }

		//keep track of mouse position for the lasso
		if (event.type == SDL_EVENT_MOUSE_MOTION)
		{
			if (mousePressed)
			{
				SDL_FPoint point;
				point.x = event.motion.x;
				point.y = event.motion.y;
				lasso.addPoint(point);
			}
		}

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			mousePressed = true;
		}
		else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			mousePressed = false;

			if(lasso.isClosed()){
				determineCaught();
			}
			
			lasso.clearPoints();
		}
	}
}

void SDLApp::handleInputFinished()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) { wantQuit = true; }

		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {

			float x = event.button.x;
			float y = event.button.y;

			if (Util::isPointInRect(retrySprite.rect, {x, y})) {
				timer = GAMETIMER;
				score = 0;
				bugs.clear();
				lasso.clearPoints();
				mousePressed = false;
				gameState = GameState::PLAYING;
			}
			else if (Util::isPointInRect(exitSprite.rect, {x, y})) {
				wantQuit = true;
			}
		}
	}
}

// DRAW FUNCTIONS

void SDLApp::drawStart()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	bgSprite.sprite.render(renderer, bgSprite.rect);
	startSprite.sprite.render(renderer, startSprite.rect);
	exitSprite.sprite.render(renderer, exitSprite.rect);

	SDL_RenderPresent(renderer);
}

void SDLApp::drawPlaying()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	
	//draw Background and Bug sprites
	bgSprite.sprite.render(renderer, bgSprite.rect);

	for (auto& bug : bugs)
	{
		bug.renderSprite(renderer);
	}

	lasso.drawLasso(renderer);

	//draw score and timer
	drawText();
	
	SDL_RenderPresent(renderer);
}

void SDLApp::drawFinished()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	bgSprite.sprite.render(renderer, bgSprite.rect);
	retrySprite.sprite.render(renderer, retrySprite.rect);
	exitSprite.sprite.render(renderer, exitSprite.rect);

	//There's a lot of magic numbers here for the text positioning i got through trial and error
	//i apologize for the ugliness
	std::string textString = "Your Score:";
	std::string scoreString = std::to_string(score);

	const float textScale = WINDOWSIZE * 0.011;
	const float scoreScale = WINDOWSIZE * 0.014;
	const float charWidth = 4.0f * scoreScale;

	const float textX = WINDOWSIZE * 0.03 / textScale;
	const float textY = WINDOWSIZE * 0.05 / textScale;
	const float scoreX = (WINDOWSIZE * 0.5 - (scoreString.size() * charWidth)) / scoreScale;
	const float scoreY = (WINDOWSIZE * 0.2) / scoreScale;
	

	SDL_SetRenderScale(renderer, textScale, textScale);
	SDL_RenderDebugText(renderer, textX, textY, textString.c_str());

	SDL_SetRenderScale(renderer, scoreScale, scoreScale);
	SDL_RenderDebugText(renderer, scoreX, scoreY, scoreString.c_str());

	SDL_SetRenderScale(renderer, 1.0f, 1.0f);
	SDL_RenderPresent(renderer);
}


void SDLApp::drawText()
{
	std::string scoreText = "Score: " + std::to_string(score);

	//convert float timer into minute/seconds
	int totalSeconds = static_cast<int>(timer);
	int minute  = static_cast<int>(totalSeconds / 60);
	int secondsTens = static_cast<int>((totalSeconds % 60) / 10);
	int secondsOnes = totalSeconds % 10;
	std::string timerText = std::to_string(minute) + ":" + std::to_string(secondsTens) + std::to_string(secondsOnes);

	const float scale = WINDOWSIZE * 0.005;
	const float charWidth = 8.0f * scale;
	const float margin = 10.0f;

	float scoreWidth = scoreText.size() * charWidth;
	float timerWidth = timerText.size() * charWidth;

	//determine the locations where the text should be rendered
	//divide by scale since the whole renderer gets scaled
	float scoreX = (WINDOWSIZE - scoreWidth - margin) / scale; 
	float timerX = (WINDOWSIZE * 0.5 - timerWidth) / scale; 
	float paddingY = margin / scale;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderScale(renderer, scale, scale);
	SDL_RenderDebugText(renderer, scoreX, paddingY, scoreText.c_str());
	SDL_RenderDebugText(renderer, timerX, paddingY, timerText.c_str());
	SDL_SetRenderScale(renderer, 1.0f, 1.0f); //reset so sprites aren't affected
}

// GAME RELATED FUNCTIONS

void SDLApp::updateGameState(float deltaTime)
{
	timer -= deltaTime;
	if (timer < 0.0f)
	{
		gameState = GameState::FINISHED;
	}
	populateBugs();
	updateBugs(deltaTime);
}

void SDLApp::populateBugs()
{
	while (bugs.size() < MAX_BUGS)
	{
		bugs.emplace_back();
		bugs.back().initRandom(renderer);
	}
}

void SDLApp::updateBugs(float deltaTime)
{
	for (int i = 0; i < static_cast<int>(bugs.size()); i++)
	{
		if (!isPointOnScreen(bugs.at(i).position))
		{
			bugs.erase(bugs.begin() + i);
			i--;
			continue;
		}
		bugs.at(i).update(deltaTime);
	}
}

void SDLApp::determineCaught()
{
	for (int i = 0; i < static_cast<int>(bugs.size()); i++)
	{
		if (lasso.isPointInLasso(bugs.at(i).position))
		{
			score += bugs.at(i).pointsAwarded;

			bugs.erase(bugs.begin() + i);
			i--;
		}
	}
}

bool SDLApp::isPointOnScreen(const glm::vec2 inPoint)
{
	float despawnMargin = SPAWNMARGIN + 1;
	
	SDL_FRect rect{
		-despawnMargin,
		-despawnMargin,
		WINDOWSIZE + (despawnMargin * 2),
		WINDOWSIZE + (despawnMargin * 2)
	};

	return Util::isPointInRect(rect, {inPoint.x, inPoint.y});
}