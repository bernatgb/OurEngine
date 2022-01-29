#include "CLight.h"

#include "Application.h"
#include "ModuleDebugDraw.h"
#include "SceneImporter.h"
#include "GameObject.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CLight::CLight(bool _enabled, GameObject* _owner) : Component(ComponentType::LIGHT, _enabled, _owner)
{
	m_Type = LightType::DIRECTIONAL;
	m_Color = float3::one;
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

void CLight::DrawImGui(int index)
{
	if (ImGui::CollapsingHeader(std::string("Light_" + std::to_string(index)).c_str()))
	{
		Component::DrawImGui(index);

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

		ImGui::Checkbox("Draw light", &m_drawLight);
		if (m_drawLight)
		{
			if (m_Type == LightType::DIRECTIONAL)
			{
				float3 from = m_Owner->m_Transform->GetPos() - m_Owner->m_Transform->GetForward() * 2;
				float3 to = m_Owner->m_Transform->GetPos() + m_Owner->m_Transform->GetForward() * 2;
				App->debugDraw->DrawDirectionalLight(from, to);
			}
			else if (m_Type == LightType::POINT)
			{
				float3 center = m_Owner->m_Transform->GetPos();
				App->debugDraw->DrawPointLight(center, m_Radius);
			}
			else if (m_Type == LightType::SPOT)
			{
				float3 apex = m_Owner->m_Transform->GetPos();
				float3 direction = m_Owner->m_Transform->GetForward() * m_Radius;
				float baseRadiusOuterCone = atan(m_OuterAngle * DEGTORAD) * m_Radius;
				float baseRadiusInnerCone = atan(m_InnerAngle * DEGTORAD) * m_Radius;
				App->debugDraw->DrawSpotLight(apex, direction, baseRadiusOuterCone, 0, float3(0.7f, 1.0f, 0.0f));
				App->debugDraw->DrawSpotLight(apex, direction, baseRadiusInnerCone, 0, float3(0.2f, 1.0f, 0.0f));
			}
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
