#include "Config.h"

#include "SceneImporter.h"

#include "rapidjson/document.h"

float3 Config::m_CamPosition = float3(0.0f, 3.0f, -8.0f);
Quat Config::m_CamRotation = float3x3::FromEulerXYZ(DEGTORAD * 15.0f, 0.0f, 0.0f).ToQuat();
float Config::m_Fov = 45.0f;
float Config::m_ZNear = 0.1f;
float Config::m_ZFar = 200.0f;

bool Config::m_DrawAxis = true;
bool Config::m_DrawGrid = true;

void Config::LoadConfig()
{
	rapidjson::Document d;
	if (importer::LoadFile(CONFIG_FILE, d)) 
	{
		// Load values
		m_CamPosition = importer::ValueToFloat3(d["CamPosition"]);
		m_CamRotation = importer::ValueToQuat(d["CamRotation"]);
		m_Fov = d["Fov"].GetFloat();
		m_ZNear = d["ZNear"].GetFloat();
		m_ZFar = d["ZFar"].GetFloat();

		m_DrawAxis = d["DrawAxis"].GetBool();
		m_DrawGrid = d["DrawGrid"].GetBool();
	}
}

void Config::SaveConfig()
{
	rapidjson::Document d;
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.SetObject();

	// Store values
	d.AddMember("CamPosition", importer::Float3ToValue(m_CamPosition, allocator), allocator);
	d.AddMember("CamRotation", importer::QuatToValue(m_CamRotation, allocator), allocator);
	d.AddMember("Fov", rapidjson::Value(m_Fov), allocator);
	d.AddMember("ZNear", rapidjson::Value(m_ZNear), allocator);
	d.AddMember("ZFar", rapidjson::Value(m_ZFar), allocator);

	d.AddMember("DrawAxis", m_DrawAxis, allocator);
	d.AddMember("DrawGrid", m_DrawGrid, allocator);

	importer::SaveFile(CONFIG_FILE, d);
}