#include "CMaterial.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


CMaterial::CMaterial(bool _enabled, GameObject* _owner) : Component(ComponentType::MATERIAL, _enabled, _owner)
{
}

CMaterial::CMaterial(bool _enabled, GameObject* _owner, Texture* _texture) : Component(ComponentType::MATERIAL, _enabled, _owner)
{
	m_Texture = _texture;
}

CMaterial::~CMaterial()
{
}

void CMaterial::ActivateMaterial()
{
	if (m_Texture == nullptr)
		return;

	m_Texture->ActivateTexture(App->renderer->program);
}

void CMaterial::DrawImGui()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		// Select material
		if (ImGui::Button("Select Material..."))
			ImGui::OpenPopup("select_material");
		ImGui::SameLine();
		if (m_Texture != nullptr) ImGui::TextUnformatted(std::to_string(m_Texture->m_GUID).c_str());
		else ImGui::TextUnformatted("No material selected");

		if (ImGui::BeginPopup("select_material"))
		{
			ImGui::Text("Material");
			ImGui::Separator();
			for (auto it = App->scene->m_Textures.begin(); it != App->scene->m_Textures.end(); ++it)
			{
				if (ImGui::Selectable(std::to_string(it->first).c_str()))
					m_Texture = it->second;
			}
			ImGui::EndPopup();
		}

		// If a material is selected
		if (m_Texture != nullptr)
		{
			m_Texture->DrawImGui();
		}
	}
}

void CMaterial::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const
{
	Component::OnSave(node, allocator);

	node.AddMember("MaterialId", rapidjson::Value(m_Texture->m_GUID), allocator);
}

void CMaterial::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	unsigned int materialId = node["MaterialId"].GetInt();
	m_Texture = App->scene->m_Textures[materialId];
}