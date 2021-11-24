#pragma once
#include "Module.h"

class ModuleImGui : public Module
{
public:
	ModuleImGui();
	~ModuleImGui();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	std::list<char*> Items;

private:
	bool showInfoWindow;
	bool showConsoleWindow;
	bool showCameraWindow;
	bool showModelWindow;
	bool showTimeWindow;

	bool autoScroll;
};