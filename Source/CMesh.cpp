#include "CMesh.h"

#include "GameObject.h"
#include "CTransform.h"
#include "GL/glew.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

CMesh::CMesh(bool _enabled, GameObject* _owner, Mesh* _mesh) : Component(ComponentType::MESH, _enabled, _owner)
{
	m_Mesh = _mesh;

	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();

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
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);

		//glBindBuffer(GL_ARRAY_BUFFER, m_VboBB);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		App->debugDraw->DrawBB(m_BB);

		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void CMesh::Disable()
{
}

void CMesh::NotifyMovement()
{
	for (unsigned int i = 0; i < 8; ++i)
		m_BB[i] = (m_Owner->m_Transform->m_AccumulativeModelMatrix * m_Mesh->m_BB[i].ToPos4()).Float3Part();
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

	//TODO: SAVE VARIABLES
}

void CMesh::OnLoad(const rapidjson::Value& node)
{
	Component::OnLoad(node);

	//TODO: LOAD VARIABLES
}
