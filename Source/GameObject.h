#pragma once
#include "Globals.h"
#include "Component.h"
#include "CTransform.h"
#include "CMesh.h"
#include "CCamera.h"
#include "CLight.h"

#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class QuadtreeNode;

class GameObject
{
public:
	GameObject(const char* _name, GameObject* _parent);
	~GameObject();

	void Start();
	void Update();
	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const;
	void OnLoad(const rapidjson::Value& node);

	GameObject* Clone(GameObject* _parent);

	bool IsInFrustum();
	bool IsInFrustumViaQuadtree(QuadtreeNode* qtn);

	void NotifyMovement(bool notifiedByTransform);

	void RecalculateBB();

	template<typename T> T* GetComponent();
	void AddComponent(Component* _newComponent);

	void SetParent(GameObject* _go);
	GameObject* AddChild(const char* _name);
	void AddChild(GameObject* _go);
	void RemoveChild(GameObject* _go);

	void DrawImGui();

	unsigned int m_GUID;
	std::string m_Name = "GameObject";
	bool m_Active;
	bool m_InFrustum;
	CTransform* m_Transform;
	std::vector<Component*> m_Components;
	GameObject* m_Parent = nullptr;
	std::vector<GameObject*> m_Children;
	AABB m_aabb;
	bool m_showAABB;

	float4 GetMaxPoint() const {
		return float4(m_Max.x, m_Max.y, m_Max.z, 1.0f);
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Max.x, m_Max.y, m_Max.z, 1.0f);
	};
	float4 GetMinPoint() const {
		return float4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
	};
	float4 GetCenter() const {
		if (m_Min.IsZero() && m_Max.IsZero())
			return m_Transform->m_AccumulativeModelMatrix * float4(0.0f, 0.0f, 0.0f, 1.0f);
		return float4(m_Min.x + (m_Max.x - m_Min.x) / 2.0f, m_Min.y + (m_Max.y - m_Min.y) / 2.0f, m_Min.z + (m_Max.z - m_Min.z) / 2.0f, 1.0f);
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x + (m_Max.x - m_Min.x) / 2.0f, m_Min.y + (m_Max.y - m_Min.y) / 2.0f, m_Min.z + (m_Max.z - m_Min.z) / 2.0f, 1.0f);
	};
	float4 GetBase() const {
		return float4(m_Min.x + (m_Max.x - m_Min.x) / 2.0f, m_Min.y, m_Min.z + (m_Max.z - m_Min.z) / 2.0f, 1.0f);
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x + (m_Max.x - m_Min.x) / 2.0f, m_Min.y, m_Min.z + (m_Max.z - m_Min.z) / 2.0f, 1.0f);
	};
	float GetDiameter() const {
		return Distance3(GetMaxPoint(), GetMinPoint());
	};

	float3 m_Min = float3::zero;
	float3 m_Max = float3::zero;

	bool m_ActiveFlag = true;
	bool m_DeleteFlag = false;
	bool m_SelectedFlag = false;

	template<typename T> static ComponentType TypeToComponentType(T type) {
		switch (T)
		{
		case CTransform:
			return ComponentType::TRANSFORM;
		case CMesh:
			return ComponentType::MESH;
		case CCamera:
			return ComponentType::CAMERA;
		case CLight:
			return ComponentType::LIGHT;
		}
		return ComponentType::UNDEFINED;
	}

	template<typename T> static T TypeToComponentType(ComponentType componentType) {
		switch (componentType)
		{
		case ComponentType::TRANSFORM:
			return CTransform;
		case ComponentType::MESH:
			return CMesh;
		case ComponentType::CAMERA:
			return CCamera;
		case ComponentType::LIGHT:
			return CLight;
		}
		return Component;
	}
};
