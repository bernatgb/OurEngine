#include "Time.h"
#include "SDL/include/SDL.h"

unsigned long int Time::m_FrameCount = 0;
double Time::m_Time = 0;
float Time::m_TimeScale = 1.0f;
double Time::m_DeltaTime = 0;
double Time::m_RealTimeDeltaTime = 0;

unsigned long int Time::m_InitialTime = SDL_GetTicks();
unsigned long int Time::m_LastFrameTime = Time::m_InitialTime;
unsigned long int Time::m_LastTime = Time::m_InitialTime;

void Time::NewFrame()
{
	++m_FrameCount;

	unsigned long int currentTime = SDL_GetTicks();

	m_DeltaTime = ((currentTime - m_LastFrameTime) * m_TimeScale) / 1000;

	m_RealTimeDeltaTime = (currentTime - m_LastFrameTime) / 1000;

	m_Time += ((currentTime - m_LastTime) * m_TimeScale) / 1000;

	m_LastFrameTime = currentTime;
	m_LastTime = currentTime;
}

double Time::GetTime()
{
	unsigned long int currentTime = SDL_GetTicks();
	m_Time += ((currentTime - m_LastTime) * m_TimeScale) / 1000;
	m_LastTime = currentTime;

	return m_Time;
}

double Time::GetRealTimeSinceStartup()
{
	return (SDL_GetTicks() - m_InitialTime) / 1000;
}

void Time::SetTimeScale(float _timeScale)
{
	m_TimeScale = _timeScale;
}

void Time::DrawImGui()
{
}
