#pragma once
#include "Component.h"

#include "MathGeoLib.h"

class CTransform : public Component
{
public:
	CTransform(bool _enabled, GameObject* _owner);
	~CTransform() override;

	void NotifyMovement();

	void DrawImGui() override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	float3 GetPos() const;
	float3 GetForward() const;
	float3 GetUp() const;
	float3 GetRight() const;

	float4x4 m_AccumulativeModelMatrix;

private:
	void RecalculateModelMatrix();

	float4x4 m_ModelMatrix;
	float3 m_Position, m_RotationEuler, m_Scale;
	Quat m_Rotation;
};

