#include "Material.h"

#include "Application.h"
#include "ModuleResources.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

Material::Material()
{
}

Material::~Material()
{
}

void Material::DrawImGui()
{
	ImGui::InputText("Material name", &m_Name);

	ImGui::ColorEdit3("Diffuse color", &m_DiffuseColor[0]);
	ImGui::ColorEdit3("Specular color", &m_SpecularColor[0]);

	ImGui::Checkbox("Use specular Alpha as shininess", &m_ShininessAlpha);
	ImGui::DragFloat("Shininess", &m_Shininess, 0.01f, 0.0f);

	ImGui::Separator();

	// DIFFUSE TEXTURE
	ImGui::Text("DiffuseTexture");

	if (ImGui::Button("Select diffuse texture..."))
		ImGui::OpenPopup("select_diffuse_texture");
	ImGui::SameLine();
	if (m_DiffuseTexture != nullptr) ImGui::TextUnformatted(m_DiffuseTexture->m_Name.c_str());
	else ImGui::TextUnformatted("No texture selected");

	if (ImGui::BeginPopup("select_diffuse_texture"))
	{
		ImGui::Text("Texture");
		ImGui::Separator();
		if (ImGui::Selectable("None")) m_DiffuseTexture = nullptr;
		for (auto it = App->resources->m_Textures.begin(); it != App->resources->m_Textures.end(); ++it)
		{
			if (ImGui::Selectable(it->second->m_Name.c_str()))
				m_DiffuseTexture = it->second;
		}
		ImGui::EndPopup();
	}

	if (m_DiffuseTexture != nullptr)
		m_DiffuseTexture->DrawImGui();

	ImGui::Separator();

	// SPECULAR TEXTURE
	ImGui::Text("SpecularTexture");

	if (ImGui::Button("Select specular texture..."))
		ImGui::OpenPopup("select_specular_texture");
	ImGui::SameLine();
	if (m_SpecularTexture != nullptr) ImGui::TextUnformatted(m_SpecularTexture->m_Name.c_str());
	else ImGui::TextUnformatted("No texture selected");

	if (ImGui::BeginPopup("select_specular_texture"))
	{
		ImGui::Text("Texture");
		ImGui::Separator();
		if (ImGui::Selectable("None")) m_SpecularTexture = nullptr;
		for (auto it = App->resources->m_Textures.begin(); it != App->resources->m_Textures.end(); ++it)
		{
			if (ImGui::Selectable(it->second->m_Name.c_str()))
				m_SpecularTexture = it->second;
		}
		ImGui::EndPopup();
	}

	if (m_SpecularTexture != nullptr)
		m_SpecularTexture->DrawImGui();

	ImGui::Separator();
}
