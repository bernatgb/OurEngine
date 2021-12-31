#pragma once
#include "Component.h"

#include "Mesh.h"

class CMesh : public Component
{
public:
	CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh);
	~CMesh() override;

	void Enable() override;
	void Update() override;
	void Disable() override;

	void NotifyMovement() override;
	void DrawImGui() override;

	float3 m_BB[8];
	float3 m_MinPoint, m_MaxPoint;
	std::vector<std::vector<float3>> m_Triangles;

private:
	Mesh* m_Mesh;


	bool m_ShowBoundingBox; // work in progress
};