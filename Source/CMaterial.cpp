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
	m_Texture->ActivateTexture(App->renderer->program);
}

void CMaterial::DrawImGui()
{
	m_Texture->DrawImGui();
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