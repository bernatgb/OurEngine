#include "Model.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "GameObject.h"

#include "GL/glew.h"

#include "assimp/ai_assert.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "ModelImporter.h"

Model::Model()
{
}

void RecursiveDeleteRoot(ModelNode* ourNode)
{
	for (unsigned int i = 0; i < ourNode->m_Children.size(); ++i)
		RecursiveDeleteRoot(ourNode->m_Children[i]);

	delete ourNode;
}

Model::~Model()
{
	//for (int i = 0; i < m_Meshes.size(); ++i)
	//	delete m_Meshes[i];

	//for (int i = 0; i < m_Textures.size(); ++i)
	//	delete m_Textures[i];

	if (m_RootStructure != nullptr)
		RecursiveDeleteRoot(m_RootStructure);
}

GameObject* Model::ExportToGO(GameObject* _parent)
{
	return RecursiveExportToGORoot(m_RootStructure, _parent);

	/*GameObject* go = _parent->AddChild(m_Name);

	if (m_Meshes.size() == 1) 
	{
		go->AddComponent(new CMesh(true, go, m_Meshes[0]));
		go->SetMaterial(m_Textures[m_Meshes[0]->m_MaterialIndex]);
	}
	else 
	{
		for (unsigned int i = 0; i < m_Meshes.size(); ++i) 
		{
			std::string name = m_Name;
			name += '_' + std::to_string(i);
			GameObject* goChild = go->AddChild(name.c_str());
			goChild->AddComponent(new CMesh(true, goChild, m_Meshes[i]));
			goChild->SetMaterial(m_Textures[m_Meshes[i]->m_MaterialIndex]);
		}
	}

	return go;*/
}

void Model::DrawImGui()
{
	ImGui::Text("Name: %s", m_Name);
	ImGui::Text("Num vertices: %i", m_NumVertices);
	ImGui::Text("Num triangles: %i", m_NumTriangles);

	if (ImGui::CollapsingHeader("Meshes"))
	{
		for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		{
			ImGui::Text("Mesh %i:", i);
			ImGui::Indent();
			m_Meshes[i]->DrawImGui();
			ImGui::Unindent();
		}
	}

	if (ImGui::CollapsingHeader("Textures"))
	{
		for (unsigned int i = 0; i < m_Textures.size(); ++i)
		{
			m_Textures[i]->DrawImGui();
		}
	}
}

GameObject* Model::RecursiveExportToGORoot(ModelNode* ourNode, GameObject* parent)
{
	GameObject* go = parent->AddChild(ourNode->m_Name.c_str());

	//TODO: USE ourNode->m_Transform

	for (unsigned int i = 0; i < ourNode->m_Meshes.size(); ++i)
		go->AddComponent(new CMesh(true, go, ourNode->m_Meshes[i], m_Textures[ourNode->m_Meshes[0]->m_MaterialIndex]));

	for (unsigned int i = 0; i < ourNode->m_Children.size(); ++i)
		RecursiveExportToGORoot(ourNode->m_Children[i], go);

	return go;
}