#include "CLight.h"

#include "SceneImporter.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CLight::CLight(bool _enabled, GameObject* _owner) : Component(ComponentType::LIGHT, _enabled, _owner)
{
	m_Type = LightType::DIRECTIONAL;
	m_Color = float3(256);
	m_Intensity = 1.0f;
	m_Radius = 5.0f;
	m_InnerAngle = 30.0f;
	m_OuterAngle = 60.0f;
}

CLight::CLight(bool _enabled, GameObject* _owner, CLight* cLight) : Component(ComponentType::LIGHT, _enabled, _owner)
{
	m_Type = cLight->m_Type;
	m_Color = cLight->m_Color;
	m_Intensity = cLight->m_Intensity;

	m_Radius = cLight->m_Radius;

	m_InnerAngle = cLight->m_InnerAngle;
	m_OuterAngle = cLight->m_OuterAngle;
}

CLight::~CLight()
{
}

Component* CLight::GetAClone(GameObject* _owner)
{
	return new CLight(m_Enabled, _owner, this);
}

void CLight::DrawImGui()
{
	if (ImGui::CollapsingHeader("Light"))
	{
		Component::DrawImGui();

		// Selected light type
		if (ImGui::Button("Light type..."))
			ImGui::OpenPopup("select_light_type");
		ImGui::SameLine();
		if (m_Type == LightType::DIRECTIONAL) ImGui::TextUnformatted("Directional");
		else if (m_Type == LightType::POINT) ImGui::TextUnformatted("Point");
		else if (m_Type == LightType::SPOT) ImGui::TextUnformatted("Spot");

		if (ImGui::BeginPopup("select_light_type"))
		{
			ImGui::Text("Light type");
			ImGui::Separator();
			if (ImGui::Selectable("Directional")) m_Type = LightType::DIRECTIONAL;
			if (ImGui::Selectable("Point")) m_Type = LightType::POINT;
			if (ImGui::Selectable("Spot")) m_Type = LightType::SPOT;
			ImGui::EndPopup();
		}

		ImGui::ColorEdit3("Light color", &m_Color[0]);
		ImGui::DragFloat("Light intensity", &m_Intensity, 0.01f, 0.0f);

		switch (m_Type)
		{
		case LightType::DIRECTIONAL:
			break;
		case LightType::POINT:
			ImGui::DragFloat("Light radius", &m_Radius, 0.01f, 0.0f);
			break;
		case LightType::SPOT:
			ImGui::DragFloat("Light radius", &m_Radius, 0.01f, 0.0f);
			ImGui::DragFloat("Light inner angle", &m_InnerAngle, 0.01f, 0.0f);
			ImGui::DragFloat("Light outer angle", &m_OuterAngle, 0.01f, 0.0f);
			break;
		}
	}
}

void CLight::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const
{
	Component::OnSave(node, allocator);

	node.AddMember("LightType", rapidjson::Value((int)m_Type), allocator);
	node.AddMember("Color", importer::Float3ToValue(m_Color, allocator), allocator);
	node.AddMember("Intensity", rapidjson::Value((int)m_Intensity), allocator);
	node.AddMember("Radius", rapidjson::Value((int)m_Radius), allocator);
	node.AddMember("InnerAngle", rapidjson::Value((int)m_InnerAngle), allocator);
	node.AddMember("OuterAngle", rapidjson::Value((int)m_OuterAngle), allocator);
}

void CLight::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	m_Type = (LightType)node["LightType"].GetInt();
	m_Color = importer::ValueToFloat3(node["Color"]);
	m_Intensity = node["Intensity"].GetFloat();
	m_Radius = node["Radius"].GetFloat();
	m_InnerAngle = node["InnerAngle"].GetFloat();
	m_OuterAngle = node["OuterAngle"].GetFloat();
}
