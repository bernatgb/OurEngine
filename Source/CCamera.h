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

	void NotifyMovement() override;
	void DrawImGui() override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	Frustum* GetCCameraFrustum();

	bool m_CurrentCamera;

	Frustum frustum;
	//TODO: ASPECT SHOULDNT BE HERE
	float aspect;

	float initialVerticalFov = 45.0f;
	//TODO: verticalFov SHOULDNT BE A VARIABLE
	float verticalFov = DEGTORAD * initialVerticalFov;
	float zNear = 0.1f;
	float zFar = 200.0f;

	//TODO: USE POS FRONT AND UP FROM THE CTRANSFORM
	float3 pos = float3(6.0f, 1.5f, -4.0f);
	float3 front = -float3::unitX;
	float3 up = float3::unitY;
};
