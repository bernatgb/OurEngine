#pragma once
#include "Module.h"

class ModuleImGui : public Module
{
public:
	ModuleImGui();
	~ModuleImGui() override;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	std::list<char*> Items;

private:
	void About(bool& show);
	void Console(bool& show);
	void SoftwareAndHardware();

	bool inspector;
	bool hierarchy;
	bool config;
	bool console;
	bool about;

	//TODO: REMOVE
	bool showInfoWindow;

	bool autoScroll;

	char* license;
};