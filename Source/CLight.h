#pragma once
#include "Component.h"

#include "MathGeoLib.h"

enum class LightType {
	DIRECTIONAL,
	POINT,
	SPOT
};

class CLight : public Component
{
public:
	CLight(bool _enabled, GameObject* _owner);
	CLight(bool _enabled, GameObject* _owner, CLight* cLight);
	~CLight() override;

	/*void Enable() override;
	void Update() override;
	void Disable() override;*/

	Component* GetAClone(GameObject* _owner) override;
	//void NotifyMovement() override;
	void DrawImGui() override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	//void OnLoad(const rapidjson::Value& node) override;

private:
	LightType m_Type;
	float3 m_Color;
	float m_Intensity;

	// Point light variables
	float m_Radius;

	// Spot light
	float m_InnerAngle;
	float m_OuterAngle;
};