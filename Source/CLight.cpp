#include "CLight.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CLight::CLight(bool _enabled, GameObject* _owner) : Component(ComponentType::LIGHT, _enabled, _owner)
{
}

CLight::~CLight()
{
}

void CLight::DrawImGui()
{
	if (ImGui::CollapsingHeader("Light"))
	{
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
