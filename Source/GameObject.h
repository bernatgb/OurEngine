#pragma once
#include "Globals.h"
#include "Component.h"
#include "CTransform.h"
#include "CMesh.h"
#include "CCamera.h"

#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

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

	void NotifyMovement();
	void NotifySonMovement();

	template<typename T> T* GetComponent();
	//template<typename T> void AddComponent(T* _newComponent);
	void AddComponent(Component* _newComponent);

	GameObject* AddChild(const char* _name);
	void DeleteChild(GameObject* _go);

	void DrawImGui();

	unsigned int m_GUID;
	char* m_Name;
	bool m_Active;
	bool m_Selected;
	bool m_InFrustum;
	CTransform* m_Transform;
	std::vector<Component*> m_Components;
	GameObject* m_Parent;
	std::vector<GameObject*> m_Children;
	AABB m_aabb;

	float4 GetMaxPoint() const {
		return float4(m_Max.x, m_Max.y, m_Max.z, 1.0f);
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Max.x, m_Max.y, m_Max.z, 1.0f);
	};
	float4 GetMinPoint() const {
		return float4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
	};
	float4 GetCenter() const {
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

	float3 m_Min;
	float3 m_Max;


	template<typename T> ComponentType TypeToComponentType(T type) {
		switch (T)
		{
		case CTransform:
			return ComponentType::TRANSFORM;
		case CMesh:
			return ComponentType::MESH;
		case CCamera:
			return ComponentType::CAMERA;
		}
		return ComponentType::UNDEFINED;
	}

	template<typename T> T TypeToComponentType(ComponentType componentType) {
		switch (componentType)
		{
		case ComponentType::TRANSFORM:
			return CTransform;
		case ComponentType::MESH:
			return CMesh;
		case ComponentType::CAMERA:
			return CCamera;
		}
		return Component;
	}
};
