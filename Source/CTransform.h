#pragma once
#include "Component.h"

#include "MathGeoLib.h"

class CTransform : public Component
{
public:
	CTransform(bool _enabled, GameObject* _owner);
	~CTransform() override;

	Component* GetAClone(GameObject* _owner) override;
	void NotifyMovement() override;
	void DrawImGui(int index) override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	void GizmoTransformChange(float4x4 _newAccumulativeModelMatrix);
	void Copy(const CTransform* _transform);

	float3 GetPos() const;
	float3 GetForward(bool normalize = false) const;
	float3 GetUp(bool normalize = false) const;
	float3 GetRight(bool normalize = false) const;
	float4x4 GetAccumulativeModelMatrix() const { return m_AccumulativeModelMatrix; };
	float4x4 m_AccumulativeModelMatrix = float4x4::identity;
	

private:
	void RecalculateModelMatrix();

	float4x4 m_ModelMatrix = float4x4::identity;
	float3 m_Position = float3::zero;
	float3 m_RotationEuler = float3::zero;
	float3 m_Scale = float3::one;
	Quat m_Rotation = Quat::identity;
};

