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

#include <string>

#include "ModelImporter.h"

Model::Model(const char* _fileName)
{
	MY_LOG("Assimp (%s): Loading the model", _fileName);

	m_Name = new char[strlen(_fileName)+1];
	strcpy(m_Name, _fileName);

	const aiScene* scene = aiImportFile(m_Name, aiProcessPreset_TargetRealtime_MaxQuality || aiProcess_Triangulate);
	if (scene)
	{
		LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		//LoadMeshes(scene->mMeshes, 1);
		LoadTextures(scene->mMaterials, scene->mNumMaterials);

		m_Min = m_Meshes[0]->m_Min;
		m_Max = m_Meshes[0]->m_Min;

		for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		{
			m_NumVertices += m_Meshes[i]->m_NumVertices;
			m_NumTriangles += m_Meshes[i]->m_NumIndices / 3;

			if (m_Meshes[i]->m_Min.x > m_Max.x) m_Max.x = m_Meshes[i]->m_Min.x;
			if (m_Meshes[i]->m_Min.x < m_Min.x) m_Min.x = m_Meshes[i]->m_Min.x;

			if (m_Meshes[i]->m_Min.y > m_Max.y) m_Max.y = m_Meshes[i]->m_Min.y;
			if (m_Meshes[i]->m_Min.y < m_Min.y) m_Min.y = m_Meshes[i]->m_Min.y;

			if (m_Meshes[i]->m_Min.z > m_Max.z) m_Max.z = m_Meshes[i]->m_Min.z;
			if (m_Meshes[i]->m_Min.z < m_Min.z) m_Min.z = m_Meshes[i]->m_Min.z;
		}

		Model* m = new Model("asdasd");
		importer::model::Import(scene, m);
		delete m;
	}
	else
	{
		MY_LOG("Error loading %s: %s", m_Name, aiGetErrorString());
	}
}

Model::~Model()
{
	delete[] m_Name;

	for (int i = 0; i < m_Meshes.size(); ++i)
		delete m_Meshes[i];

	for (int i = 0; i < m_Textures.size(); ++i)
		delete m_Textures[i];
}

GameObject* Model::ExportToGO(GameObject* _parent)
{
	GameObject* go = _parent->AddChild(m_Name);

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

	return go;
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

void Model::LoadMeshes(aiMesh** _meshes, const unsigned int& _numMeshes)
{
	MY_LOG("Assimp: Loading the meshes");
	m_Meshes = std::vector<Mesh*>(_numMeshes);

	for (unsigned int i = 0; i < _numMeshes; ++i)
	{
		MY_LOG("Assimp: Loading the mesh %i", i);
		m_Meshes[i] = new Mesh(_meshes[i]);
	}
}

void Model::LoadTextures(aiMaterial** _materials, const unsigned int& _numMaterials)
{
	MY_LOG("Assimp: Loading the textures");
	aiString file;
	m_Textures = std::vector<Texture*>(_numMaterials);
	for (unsigned i = 0; i < _numMaterials; ++i)
	{
		if (_materials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			m_Textures[i] = new Texture(file.data, m_Name);
		}
	}
}
