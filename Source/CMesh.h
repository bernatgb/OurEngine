#pragma once
#include "Component.h"

#include "Mesh.h"
#include "Material.h"

class CMesh : public Component
{
public:
	CMesh(bool _enabled, GameObject* _owner);
	CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh, Material* _material);
	~CMesh() override;

	void Enable() override;
	void Update() override;
	void Disable() override;

	Component* GetAClone(GameObject* _owner) override;
	void NotifyMovement() override;
	void DrawImGui(int index) override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	// Getters
	float3 GetMinPoint() const { return m_MinPoint; };
	float3 GetMaxPoint() const { return m_MaxPoint; };
	std::vector<Triangle> GetTriangles() const { return m_Triangles; };

	float3 m_BB[8];

private:
	// Mesh
	Mesh* m_Mesh = nullptr;
	bool m_ShowBoundingBox;
	float3 m_MinPoint, m_MaxPoint;
	std::vector<Triangle> m_Triangles;

	// Material
	Material* m_Material;
};