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

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	float3 m_BB[8];
private:
	Mesh* m_Mesh;


	bool m_ShowBoundingBox; // work in progress
};