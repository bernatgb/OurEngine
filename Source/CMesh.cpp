#include "CMesh.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CMesh::CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh) : Component(ComponentType::MESH, _enabled, _owner)
{
	m_Mesh = _mesh;
}

CMesh::~CMesh()
{
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
		m_Mesh->DrawBB();
}

void CMesh::Disable()
{
}

#include "GameObject.h"

void CMesh::DrawImGui()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Checkbox("Enabled", &m_Enabled);

		ImGui::Checkbox("Show BB", &m_ShowBoundingBox);

		//TEST
		ImGui::Text(m_Owner->m_Name);
		if (m_ShowBoundingBox)
			ImGui::Text("TRUE");
		else
			ImGui::Text("FALSE");
		//

		m_Mesh->DrawImGui();
	}
}
