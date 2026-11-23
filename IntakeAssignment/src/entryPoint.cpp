#include "SDLApp.h"

int main() 
{
	SDLApp myApp;
	Uint64 lastTime = SDL_GetPerformanceCounter();

	while (!myApp.wantQuit)
	{
		Uint64 now = SDL_GetPerformanceCounter();
		float deltaTime = static_cast<float>(now - lastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
		lastTime = now;

		myApp.update(deltaTime);
	}
}