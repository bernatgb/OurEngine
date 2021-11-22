#include "Timer.h"
#include "SDL/include/SDL.h"

Timer::Timer()
{
	stopped = false;
	startTime = 0;
	timer = 0;
}

void Timer::Start()
{
	stopped = false;
	startTime = SDL_GetTicks();
	timer = 0;
}

void Timer::Stop()
{
	stopped = true;
}

double Timer::Read()
{
	if (!stopped)
		timer = SDL_GetTicks() - startTime;
	return timer;
}
