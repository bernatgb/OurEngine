#include "AccuratedTimer.h"
#include "SDL/include/SDL.h"

uint64_t AccuratedTimer::frequency = SDL_GetPerformanceFrequency();

AccuratedTimer::AccuratedTimer()
{
	stopped = false;
	startCounter = 0;
	timerCounter = 0;
}

void AccuratedTimer::Start()
{
	stopped = false;
	startCounter = SDL_GetPerformanceCounter();
	timerCounter = 0;
}

void AccuratedTimer::Stop()
{
	stopped = true;
}

uint64_t AccuratedTimer::Read()
{
	if (!stopped)
		timerCounter = (uint64_t)((SDL_GetPerformanceCounter() - startCounter) / AccuratedTimer::frequency);
	return timerCounter;
}