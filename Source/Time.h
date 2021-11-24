#pragma once

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

	static void DrawImGui();

private:
	static unsigned long int m_FrameCount; // app graphics frames since game start
	static double m_Time; // second since game start(Game Clock)
	static float m_TimeScale; // scale at which time is passing(Game Clock)
	static double m_DeltaTime; // last frame time expressed in seconds(Game Clock)
	//static uint32_t m_RealTimeSinceStartup; // seconds since game start(Real Time Clock)
	static double m_RealTimeDeltaTime; // last frame time expressed in seconds(Real Time Clock)

	static bool m_LimitFramerate;
	static int m_MaxFPS;
	static double m_FPS;
	static double m_Delay;

	static unsigned long int m_InitialTime;
	static unsigned long int m_LastFrameTime;
	static unsigned long int m_LastTime;
};

