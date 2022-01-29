#pragma once
#include "Module.h"

#include "GameObject.h"
#include "Quadtree.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

#include <map>
#include <vector>

class Model;
class Mesh;
class Material;
class Texture;

class ModuleResources : public Module
{
public:
	ModuleResources();
	~ModuleResources() override;

	bool Init();
	bool CleanUp();

	void DrawImGuiResources();

	void LoadFile(const char* _fileName, rapidjson::Document& d);
	void LoadResource(const char* _fileName);
	void LoadModel(const char* _fileName);

	Model* FindModel(std::string _modelName);
	Mesh* FindMesh(unsigned int _meshId);
	Material* FindMaterial(unsigned int _materialId);
	Texture* FindTexture(unsigned int _textureId);

	// Getters
	std::map<std::string, Model*> const& GetModels() const
	{
		return m_Models;
	};
	std::map<unsigned int, Mesh*>  const& GetMeshes() const
	{
		return m_Meshes;
	};
	std::map<unsigned int, Material*> const& GetMaterials() const
	{
		return m_Materials;
	};
	std::map<unsigned int, Texture*> const& GetTextures() const
	{
		return m_Textures;
	};

	std::map<std::string, Model*> m_Models;
	std::map<unsigned int, Mesh*> m_Meshes;
	std::map<unsigned int, Material*> m_Materials;
	std::map<unsigned int, Texture*> m_Textures;

private:
	
};
