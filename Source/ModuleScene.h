#pragma once
#include "Module.h"

#include "Model.h"
#include "GameObject.h"

#include <vector>

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

	std::vector<Model*> models;
	int activeModel;

private:
	
	GameObject* m_Root;
	GameObject* m_GOSelected;
};
