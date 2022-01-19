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
	void DrawImGuiHierarchy();
	void DrawImGuiModel();
	void DrawImGuiResources();

	void Draw(unsigned int program);
	void LoadModel(const char* _fileName);

	void LoadTempScene();
	void SaveTempScene();
	void LoadScene(const rapidjson::Document& d);
	void SaveScene(rapidjson::Document& d);

	void AddToQuadtreeIfHasMesh(Quadtree* qt, GameObject* go);

	Model* FindModel(std::string _modelName) {
		std::map<std::string, Model*>::iterator it = m_Models.find(_modelName);
		if (it != m_Models.end())
			return it->second;
		return nullptr;
	};

	Mesh* FindMesh(unsigned int _meshId) {
		std::map<unsigned int, Mesh*>::iterator it = m_Meshes.find(_meshId);
		if (it != m_Meshes.end())
			return it->second;
		return nullptr;
	};

	Texture* FindTexture(unsigned int _textureId) {
		std::map<unsigned int, Texture*>::iterator it = m_Textures.find(_textureId);
		if (it != m_Textures.end())
			return it->second;
		return nullptr;
	};

	std::map<std::string, Model*> m_Models;
	std::map<unsigned int, Mesh*> m_Meshes;
	std::map<unsigned int, Texture*> m_Textures;

	// Gets
	const GameObject* GetRoot() const
	{
		return m_Root;
	};
	GameObject* GetSelectedGO() const
	{
		return m_GOSelected;
	};

	Quadtree* GetQuadtree() { return qt; };

private:
	GameObject* m_Root = nullptr;
	GameObject* m_GOSelected = nullptr;

	Quadtree* qt = nullptr;

	ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
	rapidjson::Document m_TempScene;
};
