#pragma once
#include "Module.h"

#include "Math/float4x4.h"
#include "Math/float3x3.h"
#include "Geometry/Frustum.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init();
	update_status Update();
	void WindowResized(unsigned width, unsigned height);
	void DrawImGui();

	float4x4 view, proj;
	float3x3 rotationMatrix;
	float3 eye, target;

private:
	void ViewProjectionMatrix();

	Frustum frustum;
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