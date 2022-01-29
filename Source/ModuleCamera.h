#pragma once
#include "Module.h"

#include "Model.h"

#include "Quadtree.h"

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
	void AdjustToGO(GameObject* _go);

	bool BoxInFrustum(Frustum const& fru, const float3* box);

	void FindIfRayIntersectsAnAABBandAddToHits(LineSegment ray, GameObject* go, std::vector<GameObject*>& hits);
	void FindIfRayIntersectsQuadtreeAreasAndAddGameObjectsToHits(LineSegment ray, QuadtreeNode* qtn, std::vector<GameObject*>& hits);
	void SortHits(std::vector<GameObject*>& hits);
	void FindIfRayIntersectsATriangle(LineSegment ray, std::vector<GameObject*>& hits, std::vector< std::pair<float, GameObject*> >& hitPointsDistances);

	bool IsMouseInScene();
	
	// Getters
	float4x4 GetView() const { return view; };
	float4x4 GetProj() const { return proj; };
	Frustum* GetFrustum();
	Frustum* GetCullingCamera();

	// Setters
	void SetCullingCamera(CCamera* _camera);

private:
	void Rotate(float _deltaPitch, float _deltaYaw);

	float4x4 view, proj;
	//float3 eye;

	CCamera* m_CullingCamera = nullptr;

	//Quat m_CameraRotation;
	float3 m_PitchYawRoll;

	Frustum frustum, gameCameraFrustum;
	float aspect;
	
	//float initialVerticalFov = 45.0f;
	float verticalFov = DEGTORAD * 45.0f;
	//float zNear = 0.1f;
	//float zFar = 200.0f;

	float speed = 10.0f;
	float mouseSpeedForRotation = 5.0f;
	float mouseSpeedForMovement = 2.5f;
	float mouseWheelSpeed = 20.0f;

	float3 rayFrom, rayTo;
	bool m_drawRay = false;
};
