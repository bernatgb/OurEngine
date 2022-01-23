#pragma once

#include "Math/Quat.h"
#include "Math/float3.h"

class Config
{
public:
	static void LoadConfig();
	static void SaveConfig();

	static float3 m_CamPosition;
	static Quat m_CamRotation;
	static float m_Fov;
	static float m_ZNear;
	static float m_ZFar;

	static bool m_DrawAxis;
	static bool m_DrawGrid;
};

