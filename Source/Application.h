#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleProgram;
class ModuleRenderExercise;
class ModuleDebugDraw;
class ModuleTexture;
class ModuleImGui;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleProgram* program = nullptr;
	ModuleRenderExercise* rendererExercise = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleImGui* imGui = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;
