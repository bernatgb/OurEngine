#pragma once
#include "Component.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"

class CCamera : public Component
{
public:
	CCamera(bool _enabled, GameObject* _owner);
	~CCamera() override;

	void Enable() override;
	void Update() override;
	void Disable() override;

	Component* GetAClone(GameObject* _owner) override;
	void NotifyMovement() override;
	void DrawImGui() override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	// Getters
	Frustum* GetCCameraFrustum();

	// Setters
	void SetCullingCamera(bool setCullingCam);
	void SetFov(float _fov);
	void SetZNearAndFar(float _zNear, float _zFar);

private:
	bool m_MainCamera = false;
	bool m_CullingCamera = false;

	Frustum frustum;

	float initialVerticalFov = 45.0f;
	float zNear = 0.1f;
	float zFar = 200.0f;
};
