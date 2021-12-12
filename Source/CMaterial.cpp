#include "CMaterial.h"

#include "Application.h"
#include "ModuleRender.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"



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
