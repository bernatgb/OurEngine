#include "ModelImporter.h"

#include "MeshImporter.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

void RecursiveRoot(Model* ourModel, aiNode* node, ModelNode* ourNode)
{
	ourNode->m_Name = node->mName.C_Str();

	for (unsigned int i = 0; i < node->mNumMeshes; ++i) 
	{
		ourNode->m_Meshes.push_back(ourModel->m_Meshes[node->mMeshes[i]]);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ourNode->m_Children.push_back(new ModelNode());
		RecursiveRoot(ourModel, node->mChildren[i], ourNode->m_Children[i]);
	}
}

void importer::model::Import(const aiScene* model, Model* ourModel)
{
	// 1. Parse a JSON string into DOM.
	const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
	rapidjson::Document d;
	d.Parse(json);

	// 2. Modify it by DOM.
	rapidjson::Value& s = d["stars"];
	s.SetInt(s.GetInt() + 1);

	MY_LOG("JSOOOOOOOOON %i", d["stars"].GetInt());

	//m_Name = new char[strlen(_fileName) + 1];
	//strcpy(m_Name, _fileName);

	//ourModel->m_Name = new char[strlen(model->GetShortFilename()) + 1];
	//strcpy(ourModel->m_Name, model->GetShortFilename());


	ourModel->m_Name = "";

	MY_LOG("Assimp: Loading the meshes");
	ourModel->m_Meshes = std::vector<Mesh*>(model->mNumMeshes);

	for (unsigned int i = 0; i < model->mNumMeshes; ++i)
	{
		MY_LOG("Assimp: Loading the mesh %i", i);
		ourModel->m_Meshes[i] = new Mesh();
		importer::mesh::Import(model->mMeshes[i], ourModel->m_Meshes[i]);
		//ourModel->m_Meshes[i] = new Mesh(model->mMeshes[i]);
	}

	MY_LOG("Assimp: Loading the textures");
	aiString file;
	ourModel->m_Textures = std::vector<Texture*>(model->mNumMaterials);
	for (unsigned i = 0; i < model->mNumMaterials; ++i)
	{
		if (model->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			ourModel->m_Textures[i] = new Texture(file.data, ".\\Assets\\Textures\\BakerHouse.png");//ourModel->m_Name);
		}
	}

	ourModel->m_Min = ourModel->m_Meshes[0]->m_Min;
	ourModel->m_Max = ourModel->m_Meshes[0]->m_Min;

	for (unsigned int i = 0; i < ourModel->m_Meshes.size(); ++i)
	{
		ourModel->m_NumVertices += ourModel->m_Meshes[i]->m_NumVertices;
		ourModel->m_NumTriangles += ourModel->m_Meshes[i]->m_NumIndices / 3;

		if (ourModel->m_Meshes[i]->m_Min.x > ourModel->m_Max.x) ourModel->m_Max.x = ourModel->m_Meshes[i]->m_Min.x;
		if (ourModel->m_Meshes[i]->m_Min.x < ourModel->m_Min.x) ourModel->m_Min.x = ourModel->m_Meshes[i]->m_Min.x;

		if (ourModel->m_Meshes[i]->m_Min.y > ourModel->m_Max.y) ourModel->m_Max.y = ourModel->m_Meshes[i]->m_Min.y;
		if (ourModel->m_Meshes[i]->m_Min.y < ourModel->m_Min.y) ourModel->m_Min.y = ourModel->m_Meshes[i]->m_Min.y;

		if (ourModel->m_Meshes[i]->m_Min.z > ourModel->m_Max.z) ourModel->m_Max.z = ourModel->m_Meshes[i]->m_Min.z;
		if (ourModel->m_Meshes[i]->m_Min.z < ourModel->m_Min.z) ourModel->m_Min.z = ourModel->m_Meshes[i]->m_Min.z;
	}

	ourModel->m_RootStructure = new ModelNode();
	RecursiveRoot(ourModel, model->mRootNode, ourModel->m_RootStructure);
}