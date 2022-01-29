#include "CCamera.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleDebugDraw.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "GameObject.h"
#include "ModuleScene.h"

CCamera::CCamera(bool _enabled, GameObject* _owner) : Component(ComponentType::CAMERA, _enabled, _owner)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(zNear, zFar);
	float aspect = App->renderer->GetSceneWindowAspect();
	float fov = DEGTORAD * initialVerticalFov;
	if (aspect < 1)
		fov = math::Atan(math::Tan(fov) / aspect);
	frustum.SetVerticalFovAndAspectRatio(fov, aspect);
	
	frustum.SetPos(m_Owner->m_Transform->GetPos());
	frustum.SetFront(m_Owner->m_Transform->GetForward());
	frustum.SetUp(m_Owner->m_Transform->GetUp());
}

CCamera::~CCamera()
{
}

void CCamera::Enable()
{
}

void CCamera::Update()
{
	if (App->renderer->HasViewportSizeChanged()) 
	{
		float aspect = App->renderer->GetSceneWindowAspect();
		float fov = DEGTORAD * initialVerticalFov;
		if (aspect < 1)
			fov = math::Atan(math::Tan(fov) / aspect);
		frustum.SetVerticalFovAndAspectRatio(fov, aspect);
	}
}

void CCamera::Disable()
{
}

Component* CCamera::GetAClone(GameObject* _owner)
{
	CCamera* newCCamera = new CCamera(m_Enabled, _owner);

	newCCamera->SetFov(initialVerticalFov);
	newCCamera->SetZNearAndFar(zNear, zFar);

	return newCCamera;
}

void CCamera::NotifyMovement()
{
	frustum.SetPos(m_Owner->m_Transform->GetPos());
	frustum.SetFront(m_Owner->m_Transform->GetForward());
	frustum.SetUp(m_Owner->m_Transform->GetUp());

	if (m_MainCamera || m_CullingCamera)
		App->camera->ViewProjectionMatrix();
}

void CCamera::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const 
{
	Component::OnSave(node, allocator);

	node.AddMember("MainCamera", rapidjson::Value(m_MainCamera), allocator);
	node.AddMember("CullingCamera", rapidjson::Value(m_CullingCamera), allocator);
	node.AddMember("Fov", rapidjson::Value(initialVerticalFov), allocator);
	node.AddMember("ZNear", rapidjson::Value(zNear), allocator);
	node.AddMember("ZFar", rapidjson::Value(zFar), allocator);
}

void CCamera::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	m_MainCamera = node["MainCamera"].GetBool();
	m_CullingCamera = node["CullingCamera"].GetBool();
	initialVerticalFov = node["Fov"].GetFloat();
	zNear = node["ZNear"].GetFloat();
	zFar = node["ZFar"].GetFloat();

	// Initialization with the new variables
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(zNear, zFar);
	float aspect = App->renderer->GetSceneWindowAspect();
	float fov = DEGTORAD * initialVerticalFov;
	if (aspect < 1)
		fov = math::Atan(math::Tan(fov) / aspect);
	frustum.SetVerticalFovAndAspectRatio(fov, aspect);

	frustum.SetPos(m_Owner->m_Transform->GetPos());
	frustum.SetFront(m_Owner->m_Transform->GetForward());
	frustum.SetUp(m_Owner->m_Transform->GetUp());
}

void CCamera::DrawImGui(int index)
{
	if (ImGui::CollapsingHeader(std::string("Camera_" + std::to_string(index)).c_str()))
	{
		Component::DrawImGui(index);

		if (ImGui::Checkbox("Main camera (in progress)", &m_MainCamera))
		{
			// TODO: IMPLEMENT
		}

		if (ImGui::Checkbox("Culling camera", &m_CullingCamera)) 
		{
			if (m_CullingCamera) App->camera->SetCullingCamera(this);
			else App->camera->SetCullingCamera(nullptr);

			if (m_MainCamera || m_CullingCamera)
				App->camera->ViewProjectionMatrix();
		}

		if (ImGui::DragFloat("Vertical FOV", &initialVerticalFov, 1.0f, 10.0f, 160.0f, "%.2f")) {
			float aspect = App->renderer->GetSceneWindowAspect();
			float fov = DEGTORAD * initialVerticalFov;
			if (aspect < 1)
				fov = math::Atan(math::Tan(DEGTORAD * initialVerticalFov) / aspect);
			frustum.SetVerticalFovAndAspectRatio(fov, aspect);

			if (m_MainCamera || m_CullingCamera)
				App->camera->ViewProjectionMatrix();
		}
			
		if (ImGui::DragFloat("Z-near", &zNear, 1.0f, 0.1f, 5.0f, "%.2f")) 
		{
			frustum.SetViewPlaneDistances(zNear, zFar);

			if (m_MainCamera || m_CullingCamera)
				App->camera->ViewProjectionMatrix();
		}

		if (ImGui::DragFloat("Z-far", &zFar, 5.0f, 6.0f, 400.0f, "%.2f"))
		{
			frustum.SetViewPlaneDistances(zNear, zFar);
			
			if (m_MainCamera || m_CullingCamera)
				App->camera->ViewProjectionMatrix();
		}

		//Draw frustum
		float3 *frustumPoints = new float3[8];
		frustum.GetCornerPoints(frustumPoints);
		App->debugDraw->DrawBB(frustumPoints);
		delete[] frustumPoints;
	}
}


// Getters
Frustum* CCamera::GetCCameraFrustum()
{
	return &frustum;
}


// Setters
void CCamera::SetCullingCamera(bool setCullingCam)
{
	m_CullingCamera = setCullingCam;
}

void CCamera::SetFov(float _fov)
{
	initialVerticalFov = _fov;

	float aspect = App->renderer->GetSceneWindowAspect();
	float fov = DEGTORAD * initialVerticalFov;
	if (aspect < 1)
		fov = math::Atan(math::Tan(DEGTORAD * initialVerticalFov) / aspect);
	frustum.SetVerticalFovAndAspectRatio(fov, aspect);
}

void CCamera::SetZNearAndFar(float _zNear, float _zFar)
{
	zNear = _zNear;
	zFar = _zFar;

	frustum.SetViewPlaneDistances(zNear, zFar);
}
