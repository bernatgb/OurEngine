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

void RecursiveTest(GameObject* gop) 
{
	for (int i = 0; i < gop->m_Children.size(); ++i)
	{
		GameObject* go = gop->m_Children[i];

		for (int j = 0; j < go->m_Components.size(); ++j)
		{
			if (go->m_Components[j]->m_Type == ComponentType::MESH)
			{
				ImGui::Text("%i:", i);
				ImGui::Text("%s", go->m_Components[j]->m_Type);
			}
		}
		RecursiveTest(go->m_Children[i]);

		ImGui::Text("%i:", i);
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

		for (int i = 0; i < App->scene->GetRoot()->m_Children.size(); ++i)
		{
			ImGui::Text("%s", App->scene->GetRoot()->m_Children[i]->m_Name);
			// get obb
			// get
		}

		ImGui::Separator();	

		RecursiveTest(App->scene->GetRoot());
		/*GameObject* go = App->scene->GetRoot()->m_Children[1];

		for (int i = 0; i < go->m_Components.size(); ++i)
		{
			if (go->m_Components[i]->m_Type == ComponentType::MESH)
			{
				ImGui::Text("%i:", i);
				ImGui::Text("%s", go->m_Components[i]->m_Type);			
			}
		}*/
		
		//ComponentType asdasd = ComponentType::CAMERA;
		//bool asdasd = asdasd == ComponentType::MESH;

		ImGui::Separator();
	}
}
