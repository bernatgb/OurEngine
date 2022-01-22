#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleImGui.h"
#include "ModuleDebugDraw.h"
#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "SDL.h"
#include "Config.h"

using namespace std;

Application::Application()
{
	Config::LoadConfig();

	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(input = new ModuleInput());
	modules.push_back(texture = new ModuleTexture());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(imGui = new ModuleImGui());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }

	delete timer;
	delete accuratedTimer;
}

bool Application::Init()
{
	timer = new Timer();
	accuratedTimer = new AccuratedTimer();
	timer->Start();
	accuratedTimer->Start();

	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	MY_LOG("Application Init took: %i ms", timer->Read());
	MY_LOG("Application Init took: %i us", accuratedTimer->Read());
	return ret;
}

update_status Application::Update()
{
	OPTICK_FRAME("NewFrame");
	Time::NewFrame();

	update_status ret = UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	Time::LimitFramerate();

	if (input->GetKey(SDL_SCANCODE_ESCAPE))
		ret = UPDATE_STOP;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	Config::SaveConfig();

	return ret;
}
