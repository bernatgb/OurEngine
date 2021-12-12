#include "CCamera.h"

#include "Application.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

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

void CCamera::DrawImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Checkbox("Current camera", &m_CurrentCamera)) 
		{
			if (m_CurrentCamera) App->camera->SetCurrentCamera(this);
			else App->camera->SetCurrentCamera(nullptr);
		}
	}
}
