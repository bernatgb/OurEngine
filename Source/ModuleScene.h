#pragma once
#include "Module.h"

#include "GameObject.h"

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

	GameObject* CreateGameObject();
	void DrawImGuiModel();

	void SelectGameObject(GameObject* go);
	void RecursiveHierarchy(GameObject* go, GameObject*& node_clicked);
	void DrawImGuiHierarchy();

	void Draw(unsigned int program);
	void LoadModel(const char* _fileName);

	const GameObject* GetRoot() const;

	std::vector<Model*> models;
	int activeModel;

	void LoadLibraryAssets();

	std::map<std::string, Model*> m_Model;
	std::map<unsigned int, Mesh*> m_Meshes;
	std::map<unsigned int, Texture*> m_Textures;

private:
	
	GameObject* m_Root;
	GameObject* m_GOSelected;
};
