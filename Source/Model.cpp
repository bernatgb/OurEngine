#include "Model.h"
#include "Application.h"
#include "ModuleTexture.h"

#include "assimp/ai_assert.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

Model::Model(const char* _fileName)
{
	const aiScene* scene = aiImportFile(_fileName, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		// TODO: LoadTextures(scene->mMaterials, scene->mNumMaterials);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		
		//LOADING MESHES
		m_Meshes = std::vector<Mesh*>(scene->mNumMeshes);

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			m_Meshes[i] = new Mesh(scene->mMeshes[i]);
		}


		//LOADING TEXTURES
		aiString file;
		m_Textures = std::vector<Texture*>(scene->mNumMaterials);
		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
			{
				m_Textures[i](App->textures->Load(file.data));
			}
		}*/

	}
	else
	{
		MY_LOG("Error loading %s: %s", file_name, aiGetErrorString());
	}
}

Model::~Model()
{
}

void Model::Draw()
{

}
