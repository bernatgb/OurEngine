#pragma once
#include "Module.h"

#include "Model.h"
#include "GameObject.h"

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene() override;

	bool Init();
	bool CleanUp();

	GameObject* CreateGameObject();
	void DrawImGuiModel();
	void DrawImGuiHierarchy();
	void Draw(unsigned int program);
	void LoadModel(const char* _fileName);

	Model* modelObj = nullptr;

private:
	
	GameObject* m_Root;
};
