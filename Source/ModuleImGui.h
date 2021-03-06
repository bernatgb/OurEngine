#pragma once
#include "Module.h"

typedef unsigned int ImGuiID;

class ModuleImGui : public Module
{
public:
	ModuleImGui();
	~ModuleImGui() override;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	void WrapMouseInWindow();

	std::list<char*> Items;

private:
	void DockSpaceUI();
	void ToolbarUI();

	ImGuiID dockSpaceId;
	float toolbarSize = 34;
	unsigned int menuHeight;

	void About(bool& show);
	void Console(bool& show);
	void SoftwareAndHardware();

	bool inspector;
	bool hierarchy;
	bool resources;
	bool config;
	bool console;
	bool about;

	bool autoScroll;

	char* license;
};
