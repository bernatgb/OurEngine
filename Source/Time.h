#pragma once
#include <vector>

class Time
{
public:
	static void NewFrame();

	static double GetTime();
	static double GetRealTimeSinceStartup();
	
	static void SetTimeScale(float _timeScale);
	static void SetLimitFramerate(bool _limit, int _FPS = 60);	

	static void LimitFramerate();

	static unsigned long int GetFrameCount() {
		return m_FrameCount;
	};
	static float GetTimeScale() {
		return m_TimeScale;
	};
	static double GetDeltaTime() {
		return m_DeltaTime;
	};
	static double GetRealTimeDeltaTime() {
		return m_RealTimeDeltaTime;
	};
	static double GetFPS() {
		return m_FPS;
	};

	static void PlayButton();
	static void PauseButton();
	static void StepButton();
	static double GetGameTime();

	static void DrawImGui();
	static void DrawImGuiToolBar();

private:
	static void Shift();

	static unsigned long int m_FrameCount; // app graphics frames since game start
	static double m_Time; // second since game start(Game Clock)
	static float m_TimeScale; // scale at which time is passing(Game Clock)
	static double m_DeltaTime; // last frame time expressed in seconds(Game Clock)
	static double m_RealTimeDeltaTime; // last frame time expressed in seconds(Real Time Clock)

	static bool m_LimitFramerate;
	static int m_MaxFPS;
	static double m_FPS;
	static double m_Delay;

	static unsigned long int m_InitialTime;
	static unsigned long int m_LastFrameTime;
	static unsigned long int m_LastTime;

	static std::vector<float> m_FPSGraph;
	static std::vector<float> m_DeltaTimeGraph;

	static double m_pause;
	static double m_pauseMoment;
	static double m_pausedTime;
	static double m_notPlayedTime;
	static bool m_gamePaused;
	static bool m_gamePlayed;
	static int m_stepFrame;
};
