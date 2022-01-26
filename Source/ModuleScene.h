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
	void DrawImGuiInspector();
	void DrawImGuiToolBar();

	void Draw(unsigned int program);
	void LoadModel(Model* _model);

	void LoadTempScene();
	void SaveTempScene();
	void LoadScene(const rapidjson::Document& d);
	void SaveScene(rapidjson::Document& d);

	void AddToQuadtreeIfHasMesh(Quadtree* qt, GameObject* go);

	void RecursiveSearch(GameObject* _go, bool ancestors, bool firstFrame = false);

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
	std::list<Light> & GetLights()
	{
		return m_Lights;
	};

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
