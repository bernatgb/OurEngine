#include "CCamera.h"

#include "Application.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "GameObject.h"
#include "ModuleScene.h"

CCamera::CCamera(bool _enabled, GameObject* _owner) : Component(ComponentType::CAMERA, _enabled, _owner)
{
	m_CurrentCamera = false;
}

CCamera::~CCamera()
{
}

void CCamera::Enable()
{
}

void CCamera::Update()
{
}

void CCamera::Disable()
{
}

void CCamera::NotifyMovement()
{
	App->camera->ViewProjectionMatrix();
}

void FindMeshes(const GameObject* gameObject) 
{
	for (int i = 0; i < gameObject->m_Components.size(); ++i)
	{
		if (gameObject->m_Components[i]->m_Type == ComponentType::MESH)
		{
			ImGui::Text("%s", "Mesh");
			CMesh* cMesh = (CMesh*)gameObject->m_Components[i];
			bool b = App->camera->BoxInFrustum(*App->camera->GetFrustum(), cMesh->m_BB);
			ImGui::SameLine();
			std::string sb = "no";
			if (b)
				sb = "yes";
			ImGui::Text("<- in frustum? %s", sb.c_str());
			// Call a function to draw the mesh, better if we do this in another part?
		}
	}

	for (int i = 0; i < gameObject->m_Children.size(); ++i)
	{
		FindMeshes(gameObject->m_Children[i]);	
	}
}

void CCamera::DrawImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Checkbox("Current camera", &m_CurrentCamera)) 
		{
			if (m_CurrentCamera) App->camera->SetCurrentCamera(this);
			else App->camera->SetCurrentCamera(nullptr);
		}

		ImGui::Separator();	

		FindMeshes(App->scene->GetRoot());

		ImGui::Separator();
	}
}
