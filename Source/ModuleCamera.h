#pragma once
#include "Module.h"

#include "Model.h"

#include "Math/float4x4.h"
#include "Math/float3x3.h"
#include "Geometry/Frustum.h"

class CCamera;

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera() override;

	bool Init();
	update_status Update();
	void WindowResized(unsigned width, unsigned height);
	void DrawImGui();

	void ViewProjectionMatrix();
	void AdjustToModel(Model* _model);

	void SetCurrentCamera(CCamera* _camera);
	
	Frustum* GetFrustum();
	Frustum* GetGameCameraFrustum();
	bool BoxInFrustum(Frustum const& fru, const float3* box);

	void FindIfRayIntersectsAnAABBandAddToHits(LineSegment ray, GameObject* go, std::vector<GameObject*> &hits);
	void SortHits(std::vector<GameObject*> &hits);
	void FindIfRayIntersectsATriangle(LineSegment ray, std::vector<GameObject*>& hits);

	float4x4 view, proj;
	float3 eye;

private:
	void Rotate(float _deltaPitch, float _deltaYaw);

	CCamera* m_CurrentCamera;

	Quat m_CameraRotation;
	float3 m_PitchYawRoll;

	Frustum frustum, gameCameraFrustum;
	float aspect;
	
	float initialVerticalFov = 45.0f;
	float verticalFov = DEGTORAD * initialVerticalFov;
	float zNear = 0.1f;
	float zFar = 200.0f;

	float speed = 10.0f;
	float mouseSpeedForRotation = 5.0f;
	float mouseSpeedForMovement = 2.5f;
	float mouseWheelSpeed = 20.0f;
};
