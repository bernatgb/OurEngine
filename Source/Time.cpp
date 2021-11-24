#include "Time.h"
#include "SDL/include/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

unsigned long int Time::m_FrameCount = 0;
double Time::m_Time = 0;
float Time::m_TimeScale = 1.0f;
double Time::m_DeltaTime = 0;
double Time::m_RealTimeDeltaTime = 0;

bool Time::m_LimitFramerate = false;
int Time::m_MaxFPS = 60;
double Time::m_FPS = 0;
double Time::m_Delay = 0;

unsigned long int Time::m_InitialTime = SDL_GetTicks();
unsigned long int Time::m_LastFrameTime = Time::m_InitialTime;
unsigned long int Time::m_LastTime = Time::m_InitialTime;

void Time::NewFrame()
{
	++m_FrameCount;

	unsigned long int currentTime = SDL_GetTicks();

	m_DeltaTime = ((currentTime - m_LastFrameTime) * m_TimeScale) / 1000.0f;

	m_RealTimeDeltaTime = (currentTime - m_LastFrameTime) / 1000.0f;

	m_Time += ((currentTime - m_LastTime) * m_TimeScale) / 1000.0f;

	m_FPS = 1.0f / m_RealTimeDeltaTime;

	m_LastFrameTime = currentTime;
	m_LastTime = currentTime;
}

double Time::GetTime()
{
	unsigned long int currentTime = SDL_GetTicks();
	m_Time += ((currentTime - m_LastTime) * m_TimeScale) / 1000.0f;
	m_LastTime = currentTime;

	return m_Time;
}

double Time::GetRealTimeSinceStartup()
{
	return (SDL_GetTicks() - m_InitialTime) / 1000.0f;
}

void Time::SetTimeScale(float _timeScale)
{
	m_TimeScale = _timeScale;
}

void Time::SetLimitFramerate(bool _limit, int _FPS)
{
	m_LimitFramerate = _limit;
	m_MaxFPS = _FPS;
}

void Time::LimitFramerate()
{
	if (m_LimitFramerate) 
	{
		m_Delay = 1.0f / m_MaxFPS - m_RealTimeDeltaTime;
		SDL_Delay(m_Delay);
	}
	else
	{
		m_Delay = 0.0f;
	}
}

void Time::DrawImGui()
{
	if (ImGui::Checkbox("", &m_LimitFramerate)) {
		SDL_GL_SetSwapInterval(m_LimitFramerate ? 0 : 1);
	}
	ImGui::SameLine();
	ImGui::SliderInt("Max FPS", &m_MaxFPS, 10, 120);

	ImGui::Text("Limiting to %i means each frame needs to take %f ms", m_MaxFPS, 1.0f / m_MaxFPS);
	ImGui::Text("Last frame we delayed for %f", m_Delay);

	ImGui::Text("Real time since start: %f; Real time dt: %f", Time::GetRealTimeSinceStartup(), Time::GetRealTimeDeltaTime());
	ImGui::Text("Game time since start: %f; Game time dt: %f", Time::GetTime(), Time::GetDeltaTime());

	ImGui::SliderFloat("Game clock scale", &m_TimeScale, 0.0f, 4.0f);

	ImGui::Text("FPS: %f; Frame count: %d", Time::GetFPS(), Time::GetFrameCount());

	//display -> histogram delta / frames
}
