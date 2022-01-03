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

	frustum = *App->camera->GetFrustum();
	frustum.SetPos(m_Owner->m_Transform->GetPos());
	frustum.SetFront(m_Owner->m_Transform->GetForward());
	frustum.SetUp(m_Owner->m_Transform->GetUp());
	frustum.SetViewPlaneDistances(zNear, zFar);
	frustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);
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
	frustum.SetPos(m_Owner->m_Transform->GetPos());
	frustum.SetFront(m_Owner->m_Transform->GetForward());
	frustum.SetUp(m_Owner->m_Transform->GetUp());

	App->camera->ViewProjectionMatrix();
}

void CCamera::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const 
{
	Component::OnSave(node, allocator);

	//TODO: SAVE VARIABLES
}

void CCamera::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	//TODO: LOAD VARIABLES
}

Frustum* CCamera::GetCCameraFrustum()
{
	return &frustum;
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

		if (ImGui::DragFloat("Vertical FOV", &initialVerticalFov, 1.0f, 10.0f, 160.0f, "%.2f")) {
			if (aspect >= 1)
				verticalFov = DEGTORAD * initialVerticalFov;
			else
				verticalFov = math::Atan(math::Tan(DEGTORAD * initialVerticalFov) / aspect);
		}
			
		if (ImGui::DragFloat("Z-near", &zNear, 1.0f, 0.1f, 5.0f, "%.2f"))
			frustum.SetViewPlaneDistances(zNear, zFar);

		if (ImGui::DragFloat("Z-far", &zFar, 5.0f, 6.0f, 400.0f, "%.2f"))
			frustum.SetViewPlaneDistances(zNear, zFar);
	}
}
