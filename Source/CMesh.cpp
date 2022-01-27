#include "CMesh.h"

#include "GameObject.h"
#include "CTransform.h"
#include "GL/glew.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleRender.h"
#include "ModuleDebugDraw.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CMesh::CMesh(bool _enabled, GameObject* _owner) : Component(ComponentType::MESH, _enabled, _owner)
{
}

CMesh::CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh, Material* _material) : Component(ComponentType::MESH, _enabled, _owner)
{
	m_Mesh = _mesh;

	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

	m_MinPoint = m_BB[5];
	m_MaxPoint = m_BB[3];

	m_Triangles = _mesh->m_Triangles;

	m_Material = _material;
}

CMesh::~CMesh()
{
}

void CMesh::Enable()
{
}

void CMesh::Update()
{
	// Return if component is not enabled
	if (!m_Enabled)
		return;

	// Return if mesh or texture are not selected
	if (m_Mesh == nullptr || m_Material == nullptr)
		return;
	
	// Return if owner not in frustrum
	if (!m_Owner->m_InFrustum)
		return;

	// Draw mesh
	App->renderer->ActivateMaterial(m_Material);
	m_Mesh->Draw();

	// Draw bounding box if needed
	if (m_ShowBoundingBox)
		App->debugDraw->DrawBB(m_BB);
}

void CMesh::Disable()
{
}

Component* CMesh::GetAClone(GameObject* _owner)
{
	return new CMesh(m_Enabled, _owner, m_Mesh, m_Material);
}

void CMesh::NotifyMovement()
{
	// If there has been a movement recalculate the bounding box
	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

	// Store new min and max point
	m_MinPoint = m_BB[5];
	m_MaxPoint = m_BB[3];
}

void CMesh::DrawImGui()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		Component::DrawImGui();

		// Selected mesh
		if (ImGui::Button("Select Mesh..."))
			ImGui::OpenPopup("select_mesh");
		ImGui::SameLine();
		if (m_Mesh != nullptr) ImGui::TextUnformatted(std::to_string(m_Mesh->m_GUID).c_str());
		else ImGui::TextUnformatted("No mesh selected");

		if (ImGui::BeginPopup("select_mesh"))
		{
			ImGui::Text("Mesh");
			ImGui::Separator();
			for (auto it = App->resources->m_Meshes.begin(); it != App->resources->m_Meshes.end(); ++it)
			{
				if (ImGui::Selectable(std::to_string(it->first).c_str()))
					m_Mesh = it->second;
			}
			ImGui::EndPopup();
		}

		// If a mesh is selected
		if (m_Mesh != nullptr) 
		{
			// Show bounding box
			ImGui::Checkbox("Show BB", &m_ShowBoundingBox);

			// Show mesh info
			m_Mesh->DrawImGui();
		}
	}
	if (ImGui::CollapsingHeader("Material"))
	{
		// Select material
		if (ImGui::Button("Select Material..."))
			ImGui::OpenPopup("select_material");
		ImGui::SameLine();
		if (m_Material != nullptr) ImGui::TextUnformatted(m_Material->m_Name.c_str());
		else ImGui::TextUnformatted("No material selected");

		if (ImGui::BeginPopup("select_material"))
		{
			ImGui::Text("Material");
			ImGui::Separator();
			for (auto it = App->resources->m_Materials.begin(); it != App->resources->m_Materials.end(); ++it)
			{
				if (ImGui::Selectable(it->second->m_Name.c_str()))
					m_Material = it->second;
			}
			ImGui::EndPopup();
		}

		// If a material is selected
		if (m_Material != nullptr)
		{
			m_Material->DrawImGui();
		}
	}
}

void CMesh::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const
{
	Component::OnSave(node, allocator);

	node.AddMember("MeshId", rapidjson::Value(m_Mesh->m_GUID), allocator);
	node.AddMember("ShowBB", rapidjson::Value(m_ShowBoundingBox), allocator);

	node.AddMember("MaterialId", rapidjson::Value(m_Material->m_GUID), allocator);
}

void CMesh::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	unsigned int meshId = node["MeshId"].GetInt();
	m_Mesh = App->resources->m_Meshes[meshId];
	m_ShowBoundingBox = node["ShowBB"].GetBool();

	unsigned int materialId = node["MaterialId"].GetInt();
	m_Material = App->resources->m_Materials[materialId];

	// Mesh initialization
	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

	m_MinPoint = m_BB[5];
	m_MaxPoint = m_BB[3];

	m_Triangles = m_Mesh->m_Triangles;
}
