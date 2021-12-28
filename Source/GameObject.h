#pragma once
#include "Globals.h"
#include "Component.h"
#include "CTransform.h"
#include "CMaterial.h"
#include "CMesh.h"

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

	bool IsInFrustum();

	void SetMaterial(Texture* _texture);
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
	CMaterial* m_Material;
	std::vector<Component*> m_Components;
	GameObject* m_Parent;
	std::vector<GameObject*> m_Children;


	float4 GetMaxPoint() const {
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Max.x, m_Max.y, m_Max.z, 1.0f);
	};
	float4 GetMinPoint() const {
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
	};
	float4 GetCenter() const {
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x + (m_Max.x - m_Min.x) / 2.0f, m_Min.y + (m_Max.y - m_Min.y) / 2.0f, m_Min.z + (m_Max.z - m_Min.z) / 2.0f, 1.0f);
	};
	float4 GetBase() const {
		return m_Transform->m_AccumulativeModelMatrix * float4(m_Min.x + (m_Max.x - m_Min.x) / 2.0f, m_Min.y, m_Min.z + (m_Max.z - m_Min.z) / 2.0f, 1.0f);
	};
	float GetDiameter() const {
		return Distance3(GetMaxPoint(), GetMinPoint());
	};

	float3 m_Min;
	float3 m_Max;
};
