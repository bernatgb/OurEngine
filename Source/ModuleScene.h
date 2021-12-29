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

	//std::vector<Model*> models;
	//int activeModel;

	void LoadLibraryAssets();
	void SaveLibraryAssets();

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

private:
	
	GameObject* m_Root;
	GameObject* m_GOSelected;
};
