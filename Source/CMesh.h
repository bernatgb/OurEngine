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

	void DrawImGui() override;

private:
	Mesh* m_Mesh;

	bool m_ShowBoundingBox; // work in progress
};