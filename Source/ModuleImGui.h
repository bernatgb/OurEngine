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
	void About();
	void Console(bool& show);

	bool inspector;
	bool hierarchy;
	bool config;
	bool console;

	//TODO: REMOVE
	bool showInfoWindow;

	bool autoScroll;
};