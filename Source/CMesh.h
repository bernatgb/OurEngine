#pragma once
#include "Component.h"

#include "Mesh.h"
#include "Texture.h"
#include "ModuleProgram.h"

class CMesh : public Component
{
public:
	CMesh(bool _enabled, GameObject* _owner);
	CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh, Texture* _texture);
	~CMesh() override;

	void Enable() override;
	void Update() override;
	void Disable() override;

	Component* GetAClone(GameObject* _owner) override;
	void NotifyMovement() override;
	void DrawImGui() override;

	void OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const override;
	void OnLoad(const rapidjson::Value& node) override;

	float3 m_BB[8];
	float3 m_MinPoint, m_MaxPoint;
	std::vector<std::vector<float3>> m_Triangles;

private:
	// Mesh
	Mesh* m_Mesh;
	bool m_ShowBoundingBox;

	// Material
	Texture* m_Texture;
	Shaders m_Shader; // currently not used
};