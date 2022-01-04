#include "CMesh.h"

#include "GameObject.h"
#include "CTransform.h"
#include "GL/glew.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CMesh::CMesh(bool _enabled, GameObject* _owner) : Component(ComponentType::MESH, _enabled, _owner)
{
}

CMesh::CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh) : Component(ComponentType::MESH, _enabled, _owner)
{
	m_Mesh = _mesh;

	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

	m_MinPoint = m_BB[6];
	m_MaxPoint = m_BB[0];

	m_Triangles = _mesh->m_Triangles;
}

CMesh::~CMesh()
{
	//delete[] m_BB;
}

void CMesh::Enable()
{
}

void CMesh::Update()
{
	if (!m_Enabled)
		return;

	m_Mesh->Draw();

	if (m_ShowBoundingBox)
		App->debugDraw->DrawBB(m_BB);
}

void CMesh::Disable()
{
}

void CMesh::NotifyMovement()
{
	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

	m_MinPoint = m_BB[6];
	m_MaxPoint = m_BB[0];

	m_Triangles = m_Mesh->m_Triangles;
}

void CMesh::DrawImGui()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Checkbox("Enabled", &m_Enabled);

		ImGui::Checkbox("Show BB", &m_ShowBoundingBox);

		m_Mesh->DrawImGui();
	}
}

void CMesh::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const
{
	Component::OnSave(node, allocator);

	node.AddMember("MeshId", rapidjson::Value(m_Mesh->m_GUID), allocator);
	node.AddMember("ShowBB", rapidjson::Value(m_ShowBoundingBox), allocator);
}

void CMesh::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	unsigned int meshId = node["MeshId"].GetInt();
	m_Mesh = App->scene->m_Meshes[meshId];
	m_ShowBoundingBox = node["ShowBB"].GetInt();

	// Mesh initialization
	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

	m_MinPoint = m_BB[6];
	m_MaxPoint = m_BB[0];

	m_Triangles = m_Mesh->m_Triangles;
}
