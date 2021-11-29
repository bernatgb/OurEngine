#include "Model.h"
#include "Application.h"
#include "ModuleTexture.h"

#include "GL/glew.h"

#include "assimp/ai_assert.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

Model::Model(const char* _fileName)
{
	MY_LOG("Assimp (%s): Loading the model", _fileName);

	m_Name = new char[strlen(_fileName)+1];
	strcpy(m_Name, _fileName);

	m_ModelMatrix = float4x4::identity;
	m_Position = float3(0.0f, 0.0f, 0.0f);
	m_RotationEuler = float3(0.0f, 0.0f, 0.0f);
	m_Rotation = Quat::identity;
	m_Scale = float3(1.0f, 1.0f, 1.0f);

	const aiScene* scene = aiImportFile(m_Name, aiProcessPreset_TargetRealtime_MaxQuality || aiProcess_Triangulate);
	if (scene)
	{
		LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		LoadTextures(scene->mMaterials, scene->mNumMaterials);

		m_MinX = m_Meshes[0]->GetMinX();
		m_MaxX = m_Meshes[0]->GetMaxX();
		m_MinY = m_Meshes[0]->GetMinY();
		m_MaxY = m_Meshes[0]->GetMaxY();
		m_MinZ = m_Meshes[0]->GetMinZ();
		m_MaxZ = m_Meshes[0]->GetMaxZ();

		for (unsigned int i = 0; i < m_Meshes.size(); ++i)
		{
			m_NumVertices += m_Meshes[i]->GetNumVertices();
			m_NumTriangles += m_Meshes[i]->GetNumIndices() / 3;

			if (m_Meshes[i]->GetMaxX() > m_MaxX) m_MaxX = m_Meshes[i]->GetMaxX();
			if (m_Meshes[i]->GetMinX() < m_MinX) m_MinX = m_Meshes[i]->GetMinX();

			if (m_Meshes[i]->GetMaxY() > m_MaxY) m_MaxY = m_Meshes[i]->GetMaxY();
			if (m_Meshes[i]->GetMinY() < m_MinY) m_MinY = m_Meshes[i]->GetMinY();

			if (m_Meshes[i]->GetMaxZ() > m_MaxZ) m_MaxZ = m_Meshes[i]->GetMaxZ();
			if (m_Meshes[i]->GetMinZ() < m_MinZ) m_MinZ = m_Meshes[i]->GetMinZ();
		}
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

void Model::Draw(const unsigned int& _program) const
{
	float4x4 model = m_ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_TRUE, &model[0][0]);

	for (unsigned int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Textures[m_Meshes[i]->GetMaterialIndex()]->ActivateTexture(_program);
		m_Meshes[i]->Draw();
	}
}

void Model::DrawImGui()
{
	ImGui::Text("Name: %s", m_Name);
	ImGui::Text("Num vertices: %i", m_NumVertices);
	ImGui::Text("Num triangles: %i", m_NumTriangles);
	ImGui::Text("Textures:");
	for (unsigned int i = 0; i < m_Textures.size(); ++i)
	{
		m_Textures[i]->DrawImGui();
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("In progress");

		if (ImGui::SliderFloat3("Position", &m_Position[0], -10.0f, 10.0f))
		{
			m_ModelMatrix = float4x4::FromTRS(m_Position, m_Rotation, m_Scale);
		}
		if (ImGui::SliderFloat3("Rotation", &m_RotationEuler[0], 0.0f, 360.0f))
		{
			m_Rotation = Quat::FromEulerXYZ(m_RotationEuler.x * DEGTORAD, m_RotationEuler.y * DEGTORAD, m_RotationEuler.z * DEGTORAD);
			m_ModelMatrix = float4x4::FromTRS(m_Position, m_Rotation, m_Scale);
		}
		if (ImGui::SliderFloat3("Scale", &m_Scale[0], -10.0f, 10.0f))
		{
			m_ModelMatrix = float4x4::FromTRS(m_Position, m_Rotation, m_Scale);
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
