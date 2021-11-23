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

uint32_t Timer::Read()
{
	if (!stopped)
		timer = SDL_GetTicks() - startTime;
	return timer;
}
