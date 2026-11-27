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

	lasso.init(renderer, SCALEDIFF);

	//init Background / Menu sprites
	bgSprite.rect = { 0, 0, WINDOWSIZE, WINDOWSIZE };
	bgSprite.sprite.init(renderer, "./Images/Grass.bmp");

	const float midScreen = WINDOWSIZE * 0.5;
	const float quarterScreen = WINDOWSIZE * 0.25;
	const float threeEighths = WINDOWSIZE * 0.375;

	SDL_FRect middleRect{ quarterScreen, threeEighths, midScreen, quarterScreen };

	startSprite.rect = middleRect;
	startSprite.sprite.init(renderer, "./Images/start_box.bmp");

	retrySprite.rect = middleRect;
	retrySprite.sprite.init(renderer, "./Images/retry_box.bmp");

	continueSprite.rect = middleRect;
	continueSprite.sprite.init(renderer, "./Images/continue_box.bmp");

	exitSprite.rect = { threeEighths, midScreen + (midScreen / 3), quarterScreen, quarterScreen * 0.5f };
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
	case GameState::PAUSED:
		handleInputPaused();
		drawPaused();
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
			SDL_FPoint click{ event.button.x, event.button.y };
			//start button clicked
			if (Util::isPointInRect(startSprite.rect, click)) {
				gameState = GameState::PLAYING;
			}
			//exit button clicked
			else if (Util::isPointInRect(exitSprite.rect, click)) {
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
				SDL_FPoint point{ event.button.x, event.button.y };
				lasso.addPoint(point);
			}
		}

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
			mousePressed = true;
			lasso.clearPoints();
		}
		else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			mousePressed = false;

			if(lasso.isClosed()){
				determineCaught();
			}
			else{
				lasso.startFade();
			}
		}

		if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
			lasso.clearPoints();
			mousePressed = false;
			gameState = GameState::PAUSED;
		}
	}
}

void SDLApp::handleInputFinished()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) { wantQuit = true; }

		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
			SDL_FPoint click{ event.button.x, event.button.y };

			if (Util::isPointInRect(retrySprite.rect, click)) {
				timer = GAMETIMER;
				score = 0;
				bugs.clear();
				lasso.clearPoints();
				mousePressed = false;
				gameState = GameState::PLAYING;
			}
			else if (Util::isPointInRect(exitSprite.rect, click)) {
				wantQuit = true;
			}
		}
	}
}

void SDLApp::handleInputPaused()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) { wantQuit = true; }

		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {	
			SDL_FPoint click{ event.button.x, event.button.y };

			if (Util::isPointInRect(continueSprite.rect, click)) {
				gameState = GameState::PLAYING;
			}
			else if (Util::isPointInRect(exitSprite.rect, click)) {
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
	SDL_RenderClear(renderer);
	
	bgSprite.sprite.render(renderer, bgSprite.rect);

	for (auto& bug : bugs)
	{
		bug.renderSprite(renderer);
	}

	lasso.drawLasso(renderer);
	drawTimerAndScore();

	if (effectTimer > 0.0f){
		std::string text = "+" + std::to_string(effectCount);
		Util::drawText(renderer, text, effectPos.x / EFFECTSCALE, effectPos.y / EFFECTSCALE, EFFECTSCALE);
	}
	
	SDL_RenderPresent(renderer);
}

void SDLApp::drawFinished()
{
	SDL_RenderClear(renderer);

	bgSprite.sprite.render(renderer, bgSprite.rect);
	retrySprite.sprite.render(renderer, retrySprite.rect);
	exitSprite.sprite.render(renderer, exitSprite.rect);

	//There's a lot of magic numbers here for the text positioning i got through trial and error
	//i apologize for the ugliness
	const std::string textString = "Your Score:";
	const std::string scoreString = std::to_string(score);

	const float textScale = WINDOWSIZE * 0.011;
	const float scoreScale = WINDOWSIZE * 0.014;
	const float charWidth = 4.0f * scoreScale;

	const float textX = WINDOWSIZE * 0.03 / textScale;
	const float textY = WINDOWSIZE * 0.05 / textScale;
	const float scoreX = (WINDOWSIZE * 0.5 - (scoreString.size() * charWidth)) / scoreScale;
	const float scoreY = (WINDOWSIZE * 0.2) / scoreScale;

	Util::drawText(renderer, textString, textX, textY, textScale);
	Util::drawText(renderer, scoreString, scoreX, scoreY, scoreScale);

	SDL_RenderPresent(renderer);
}

void SDLApp::drawPaused()
{
	SDL_RenderClear(renderer);

	bgSprite.sprite.render(renderer, bgSprite.rect);
	continueSprite.sprite.render(renderer, retrySprite.rect);
	exitSprite.sprite.render(renderer, exitSprite.rect);

	drawTimerAndScore();

	SDL_RenderPresent(renderer);
}


void SDLApp::drawTimerAndScore()
{
	const std::string scoreText = "Score: " + std::to_string(score);

	//convert float timer into minute/seconds
	const int totalSeconds = static_cast<int>(timer);
	const int minute  = static_cast<int>(totalSeconds / 60);
	const int secondsTens = static_cast<int>((totalSeconds % 60) / 10);
	const int secondsOnes = totalSeconds % 10;
	const std::string timerText = std::to_string(minute) + ":" + std::to_string(secondsTens) + std::to_string(secondsOnes);
	//i split up the two digits so it renders single seconds as "1:04" instead of "1:4" (for example)

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

	Util::drawText(renderer, scoreText, scoreX, paddingY, scale);
	Util::drawText(renderer, timerText, timerX, paddingY, scale);
}

// GAME RELATED FUNCTIONS

void SDLApp::updateGameState(float deltaTime)
{
	timer -= deltaTime;
	if (timer < 0.0f){
		gameState = GameState::FINISHED;
	}
	if (effectTimer > 0.0f) {
		effectTimer -= deltaTime;
	}

	populateBugs();
	updateBugs(deltaTime);
	lasso.update(deltaTime);
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
		//despawn if they leave the screen
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
	int count = 0;
	for (int i = 0; i < static_cast<int>(bugs.size()); i++)
	{
		if (lasso.isPointInLasso(bugs.at(i).position))
		{
			count += bugs.at(i).pointsAwarded;
			
			bugs.erase(bugs.begin() + i);
			i--;
		}
	}

	score += count;
	
	if (count > 0) {
		lasso.startConfirm();
		effectPos = lasso.getCenter();
		effectCount = count;
		effectTimer = EFFECTTIMER;
	}
	else {
		lasso.startFade();
	}
}

bool SDLApp::isPointOnScreen(const glm::vec2 inPoint)
{
	const float despawnMargin = SPAWNMARGIN + 1;
	
	SDL_FRect rect{
		-despawnMargin,
		-despawnMargin,
		WINDOWSIZE + (despawnMargin * 2),
		WINDOWSIZE + (despawnMargin * 2)
	};
	return Util::isPointInRect(rect, {inPoint.x, inPoint.y});
}