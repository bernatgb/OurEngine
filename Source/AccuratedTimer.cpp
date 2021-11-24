#include "AccuratedTimer.h"
#include "SDL/include/SDL.h"

unsigned long long int AccuratedTimer::frequency = SDL_GetPerformanceFrequency();

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

unsigned long long int AccuratedTimer::Read()
{
	if (!stopped)
		timerCounter = (unsigned long long int)((SDL_GetPerformanceCounter() - startCounter) / AccuratedTimer::frequency);
	return timerCounter;
}