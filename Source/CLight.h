#pragma once
#include "Component.h"

#include "MathGeoLib.h"

enum class LightType {
	DIRECTIONAL = 0,
	POINT = 1,
	SPOT = 2
};

struct Light 
{
public:
	int lightType;
	int padding1[3];
	float3 lightColor;
	int padding2[1];
	float intensity;
	float radius;
	float innerAngle;
	float outerAngle;

	float3 direction;
	int padding3[1];
	float3 position;
	int padding4[1];
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
	void OnLoad(const rapidjson::Value& node) override;

	Light GetLightStruct(float3 _direction, float3 _position) {
		Light light;
		light.lightType = (int)m_Type;
		light.lightColor = m_Color;
		light.intensity = m_Intensity;
		light.radius = m_Radius;
		light.innerAngle = m_InnerAngle * DEGTORAD;
		light.outerAngle = m_OuterAngle * DEGTORAD;

		light.direction = _direction; // m_Owner->m_Transform->GetForward(true);
		light.position = _position; // m_Owner->m_Transform->GetPos();

		return light;
	};

private:
	LightType m_Type = LightType::DIRECTIONAL;
	float3 m_Color = float3::one;
	float m_Intensity = 1.0f;

	// Point light variables
	float m_Radius = 5.0f;

	// Spot light
	float m_InnerAngle = 30.0f;
	float m_OuterAngle = 60.0f;
};