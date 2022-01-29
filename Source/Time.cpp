#include "Time.h"
#include "SDL/include/SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "GL/glew.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleScene.h"

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

std::vector<float> Time::m_FPSGraph = std::vector<float>(100);
std::vector<float> Time::m_DeltaTimeGraph = std::vector<float>(100);

double Time::m_pause = 0;
double Time::m_pauseMoment = 0;
double Time::m_pausedTime = 0;
double Time::m_notPlayedTime = 0;
bool Time::m_gamePaused = false;
bool Time::m_gamePlayed = false;
int Time::m_stepFrame = 0;

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

	Shift();
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
		m_Delay = 1000.0f / m_MaxFPS - (SDL_GetTicks() - m_LastFrameTime);
		if (m_Delay > 0.0f)
			SDL_Delay(m_Delay);
	}
	else
	{
		m_Delay = 0.0f;
	}
}

void Time::PlayButton()
{
	m_pausedTime += GetTime() - m_pause;
}

void Time::PauseButton()
{
	m_pause = GetTime();
	m_stepFrame = GetFrameCount();
}

void Time::StepButton()
{
	// TO THINK ON IT
	// FrameCount should stop in pause?
}

double Time::GetGameTime()
{
	if (!m_gamePlayed)
	{
		return 0;
	}
	else 
	{
		if (m_gamePaused)
		{
		return m_pauseMoment;
		}
		else
		{
			return GetTime() - (m_notPlayedTime + m_pausedTime);
		}
	}
}



void Time::DrawImGui()
{
	if (ImGui::CollapsingHeader("Time"))
	{
		if (ImGui::Checkbox("Fix", &m_LimitFramerate)) {
			SDL_GL_SetSwapInterval(m_LimitFramerate ? 0 : 1);
		}
		ImGui::SameLine();
		ImGui::SliderInt("Max FPS", &m_MaxFPS, 10, 120);
		
		ImGui::Text("Limiting to %i means each frame needs to take %f ms", m_MaxFPS, 1.0f / m_MaxFPS);
		ImGui::Text("Last frame we delayed for %f", m_Delay);

		ImGui::Text("Real time since start: %f; Real time dt: %f", Time::GetRealTimeSinceStartup(), Time::GetRealTimeDeltaTime());
		ImGui::Text("Game time since start: %f; Game time dt: %f", Time::GetGameTime(), Time::GetDeltaTime());

		// TODO: Add images, put them all in an "Init()" & here only call them.

		ImGui::SliderFloat("Game clock scale", &m_TimeScale, 0.0f, 4.0f);

		ImGui::Text("FPS: %f; Frame count: %d", Time::GetFPS(), Time::GetFrameCount());

		ImGui::PlotHistogram("##framerate", &m_FPSGraph[0], m_FPSGraph.size(), 0, "FPS", 0.0f, 120.0f);
		ImGui::PlotHistogram("##framerate", &m_DeltaTimeGraph[0], m_DeltaTimeGraph.size(), 0, "DeltaTime", 0.0f);
	}
}

void Time::DrawImGuiToolBar()
{
	if (ImGui::Button("Play |>"))
	{
		if (!m_gamePlayed)
		{
			App->scene->SaveTempScene();
			m_notPlayedTime = GetTime();
			m_pausedTime = 0;
			m_gamePaused = false;
		}
		else
		{
			App->scene->LoadTempScene();
		}
		m_gamePlayed = !m_gamePlayed;
	}

	ImGui::SameLine();

	if (ImGui::Button("Pause ||"))
	{
		if (m_gamePlayed)
		{
			if (!m_gamePaused)
			{
				m_pauseMoment = GetGameTime();
				PauseButton();
			}
			else
				PlayButton();

			m_gamePaused = !m_gamePaused;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Step |>| (decorative)"))
	{
		// To Revise
		ImGui::SetTooltip("Pause the game clock before");
		if (m_gamePlayed)
			if (m_gamePaused)
				StepButton();
	}


	if (m_gamePlayed)
	{
		ImGui::SameLine();
		ImGui::Text("PlayMode!");
	}
}

void Time::Shift() 
{
	int lenght = m_FPSGraph.size() - 1;
	for (int i = 0; i < lenght; ++i)
	{
		m_FPSGraph[i] = m_FPSGraph[i + 1];
		m_DeltaTimeGraph[i] = m_DeltaTimeGraph[i + 1];
	}

	m_FPSGraph[lenght] = m_FPS;
	m_DeltaTimeGraph[lenght] = m_RealTimeDeltaTime;
}
