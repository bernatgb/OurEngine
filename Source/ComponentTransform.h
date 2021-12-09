#pragma once
#include "Component.h"

#include "MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform() override;

	void Enable() override;
	void Update() override;
	void Disable() override;

	float4x4 m_AccumulativeModelMatrix;

	float4x4 m_ModelMatrix;
	float3 m_Position, m_RotationEuler, m_Scale;
	Quat m_Rotation;
};

