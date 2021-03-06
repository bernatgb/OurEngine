#include "ModelImporter.h"

#include "SceneImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"

#include "Application.h"
#include "ModuleResources.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

void RecursiveRoot(const Model* ourModel, const aiNode* node, ModelNode* ourNode)
{
	//Store node name
	ourNode->m_Name = node->mName.C_Str();

	//Store node transform
	ourNode->m_Transform = float4x4(
		(float)node->mTransformation[0][0], (float)node->mTransformation[0][1], (float)node->mTransformation[0][2], (float)node->mTransformation[0][3],
		(float)node->mTransformation[1][0], (float)node->mTransformation[1][1], (float)node->mTransformation[1][2], (float)node->mTransformation[1][3],
		(float)node->mTransformation[2][0], (float)node->mTransformation[2][1], (float)node->mTransformation[2][2], (float)node->mTransformation[2][3],
		(float)node->mTransformation[3][0], (float)node->mTransformation[3][1], (float)node->mTransformation[3][2], (float)node->mTransformation[3][3]
	);

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

void importer::model::Import(const aiScene* model, Model* ourModel, std::string fullPath)
{
	MY_LOG("Assimp (%s): Loading the model", fullPath);
	ourModel->m_Name = fullPath;
	const size_t last_slash_idx = ourModel->m_Name.rfind('\\');
	if (std::string::npos != last_slash_idx)
		ourModel->m_Name = ourModel->m_Name.substr(last_slash_idx+1, ourModel->m_Name.length());

	MY_LOG("Assimp: Loading the meshes");
	ourModel->m_Meshes = std::vector<Mesh*>(model->mNumMeshes);

	for (unsigned int i = 0; i < model->mNumMeshes; ++i)
	{
		MY_LOG("Assimp: Loading the mesh %i", i);
		ourModel->m_Meshes[i] = new Mesh();
		importer::mesh::Import(model->mMeshes[i], ourModel->m_Meshes[i]);
		App->resources->m_Meshes[ourModel->m_Meshes[i]->m_GUID] = ourModel->m_Meshes[i];
	}

	MY_LOG("Assimp: Loading the textures");
	aiString file;
	ourModel->m_Materials = std::vector<Material*>(model->mNumMaterials);
	for (unsigned i = 0; i < model->mNumMaterials; ++i)
	{
		MY_LOG("Assimp: Loading the material %i", i);
		ourModel->m_Materials[i] = new Material();
		importer::material::Import(model->mMaterials[i], ourModel->m_Materials[i], fullPath.c_str());
		App->resources->m_Materials[ourModel->m_Materials[i]->m_GUID] = ourModel->m_Materials[i];
	}

	ourModel->m_Min = (ourModel->m_Meshes.size() >= 1) ? ourModel->m_Meshes[0]->m_Min : float3::zero;
	ourModel->m_Max = (ourModel->m_Meshes.size() >= 1) ? ourModel->m_Meshes[0]->m_Min : float3::zero;

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
	ourModel->m_RootStructure->m_Name = ourModel->m_Name;
}

void RecusiveNodeToJson(rapidjson::Value& node, const ModelNode* modelNode, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value nodeName(modelNode->m_Name.c_str(), allocator);

	rapidjson::Value transform(rapidjson::kArrayType);
	for (unsigned int i = 0; i < 4; ++i) 
	{
		for (unsigned int j = 0; j < 4; ++j)
			transform.PushBack(modelNode->m_Transform[i][j], allocator);
	}

	rapidjson::Value meshes(rapidjson::kArrayType);
	for (unsigned int i = 0; i < modelNode->m_Meshes.size(); ++i)
		meshes.PushBack(modelNode->m_Meshes[i]->m_GUID, allocator);

	rapidjson::Value children(rapidjson::kArrayType);
	for (unsigned int i = 0; i < modelNode->m_Children.size(); ++i)
	{
		rapidjson::Value newNode(rapidjson::kObjectType);
		RecusiveNodeToJson(newNode, modelNode->m_Children[i], allocator);
		children.PushBack(newNode, allocator);
	}

	node.AddMember("Name", nodeName, allocator);
	node.AddMember("Transform", transform, allocator);
	node.AddMember("Meshes", meshes, allocator);
	node.AddMember("Children", children, allocator);
}

int importer::model::Save(const Model* ourModel, char*& fileBuffer)
{
	rapidjson::Document d;
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.SetObject();

	rapidjson::Value name(ourModel->m_Name.c_str(), allocator);
	d.AddMember("Name", name, allocator);

	d.AddMember("NumVertices", rapidjson::Value(ourModel->m_NumVertices), allocator);

	d.AddMember("NumTriangles", rapidjson::Value(ourModel->m_NumTriangles), allocator);

	rapidjson::Value meshes(rapidjson::kArrayType);
	for (unsigned int i = 0; i < ourModel->m_Meshes.size(); ++i)
		meshes.PushBack(ourModel->m_Meshes[i]->m_GUID, allocator);
	d.AddMember("Meshes", meshes, allocator);

	rapidjson::Value textures(rapidjson::kArrayType);
	for (unsigned int i = 0; i < ourModel->m_Materials.size(); ++i)
		textures.PushBack(ourModel->m_Materials[i]->m_GUID, allocator);
	d.AddMember("Materials", textures, allocator);

	d.AddMember("Min", Float3ToValue(ourModel->m_Min, allocator), allocator);
	d.AddMember("Max", Float3ToValue(ourModel->m_Max, allocator), allocator);

	rapidjson::Value root(rapidjson::kObjectType);
	RecusiveNodeToJson(root, ourModel->m_RootStructure, allocator);
	d.AddMember("Root", root, allocator);

	//TEST
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);

	const char* jsonFile = buffer.GetString();
	unsigned int size = strlen(jsonFile);

	fileBuffer = new char[size + 1];
	strcpy(fileBuffer, jsonFile);
	//

	return size;
}

void RecusiveNodeFromJson(const rapidjson::Value& node, ModelNode*& modelNode) {
	modelNode = new ModelNode();

	//Get name
	rapidjson::Value::ConstMemberIterator it = node.MemberBegin();
	modelNode->m_Name = it->value.GetString();
	++it;

	//Get transform
	rapidjson::Value::ConstValueIterator transItr = it->value.Begin();
	modelNode->m_Transform = float4x4::identity;
	for (unsigned int i = 0; i < 4; ++i)
	{
		for (unsigned int j = 0; j < 4; ++j) 
		{
			modelNode->m_Transform[i][j] = transItr->GetFloat();
			++transItr;
		}
	}
	++it;

	//Get meshes
	for (rapidjson::Value::ConstValueIterator itr = it->value.Begin(); itr != it->value.End(); ++itr)
	{
		Mesh* mesh = App->resources->FindMesh(itr->GetInt());
		if (mesh == nullptr)
		{
			MY_LOG("Error when loading Model");
		}
		modelNode->m_Meshes.push_back(mesh);
	}
	++it;

	//Get children
	for (rapidjson::Value::ConstValueIterator itr = it->value.Begin(); itr != it->value.End(); ++itr)
	{
		ModelNode* newNode = nullptr;
		RecusiveNodeFromJson(*itr, newNode);
		modelNode->m_Children.push_back(newNode);
	}
	++it;
}

void importer::model::Load(const char* fileBuffer, Model* ourModel)
{
	rapidjson::Document d;
	d.Parse(fileBuffer);

	ourModel->m_Name = d["Name"].GetString();
	ourModel->m_NumVertices = d["NumVertices"].GetInt();
	ourModel->m_NumTriangles = d["NumTriangles"].GetInt();

	for (rapidjson::Value::ConstValueIterator itr = d["Meshes"].Begin(); itr != d["Meshes"].End(); ++itr)
	{
		Mesh* mesh = App->resources->FindMesh(itr->GetInt());
		if (mesh == nullptr) 
		{
			MY_LOG("Error when loading Model");
		}
		ourModel->m_Meshes.push_back(mesh);
	}

	for (rapidjson::Value::ConstValueIterator itr = d["Materials"].Begin(); itr != d["Materials"].End(); ++itr)
	{
		Material* material = App->resources->FindMaterial(itr->GetInt());
		if (material == nullptr)
		{
			MY_LOG("Error when loading Model");
		}
		ourModel->m_Materials.push_back(material);
	}

	ourModel->m_Min = ValueToFloat3(d["Min"]);
	ourModel->m_Max = ValueToFloat3(d["Max"]);

	RecusiveNodeFromJson(d["Root"], ourModel->m_RootStructure);
	ourModel->m_RootStructure->m_Name = ourModel->m_Name;
}

