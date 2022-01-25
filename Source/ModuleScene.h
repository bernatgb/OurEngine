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

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene() override;

	bool Init();
	bool CleanUp();

	GameObject* CreateGameObject(const char* _name);

	void SelectGameObject(GameObject* go);
	void RecursiveHierarchy(GameObject* go, GameObject*& node_clicked);
	void CheckHoverHierarchy(GameObject* go);
	void DrawImGuiHierarchy();
	void DrawImGuiModel();
	void DrawImGuiResources();
	void DrawImGuiToolBar();

	void Draw(unsigned int program);
	void LoadResource(const char* _fileName);
	void LoadModel(const char* _fileName);

	void LoadTempScene();
	void SaveTempScene();
	void LoadScene(const rapidjson::Document& d);
	void SaveScene(rapidjson::Document& d);

	void AddToQuadtreeIfHasMesh(Quadtree* qt, GameObject* go);

	void RecursiveSearch(GameObject* _go, bool ancestors, bool firstFrame = false);

	Model* FindModel(std::string _modelName); // TODO: delete?
	Mesh* FindMesh(unsigned int _meshId);
	Material* FindMaterial(unsigned int _materialId);
	Texture* FindTexture(unsigned int _textureId);

	// Getters
	GameObject* GetRoot() const
	{
		return m_Root;
	};
	GameObject* GetSelectedGO() const
	{
		return m_GOSelected;
	};
	Quadtree* GetQuadtree() const
	{ 
		return qt; 
	};
	std::map<std::string, Model*> GetModels()
	{
		return m_Models;
	};
	std::map<unsigned int, Mesh*> GetMeshes()
	{
		return m_Meshes;
	};
	std::map<unsigned int, Material*> GetMaterials()
	{
		return m_Materials;
	};
	std::map<unsigned int, Texture*> GetTextures()
	{
		return m_Textures;
	};
	std::list<Light> GetLights()
	{
		return m_Lights;
	};


	std::map<std::string, Model*> m_Models;
	std::map<unsigned int, Mesh*> m_Meshes;
	std::map<unsigned int, Material*> m_Materials;
	std::map<unsigned int, Texture*> m_Textures;

	std::list<Light> m_Lights;

private:
	GameObject* m_Root = nullptr;
	GameObject* m_GOSelected = nullptr;
	GameObject* m_GODrag = nullptr;
	GameObject* m_GODragParent = nullptr;

	Quadtree* qt = nullptr;

	ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
	rapidjson::Document m_TempScene;
};
