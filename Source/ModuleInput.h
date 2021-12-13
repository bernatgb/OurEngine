#pragma once
#include "Module.h"

#include "SDL/include/SDL.h"

typedef unsigned __int8 Uint8;

#define NUM_MOUSE_BUTTONS 5

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput() override;

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	const Uint8 GetKey(SDL_Scancode key) const;
	const bool GetMouseButton(unsigned int key) const;
	const void GetMouseMotion(int& x, int& y) const;

	const bool MouseWheel() const { return mouse_wheel; };
	const void GetMouseWheel(int& x, int& y) const { x = mouse_wheel_x; y = mouse_wheel_y; };

private:
	const Uint8 *keyboard = NULL;
	bool mouse_buttons[NUM_MOUSE_BUTTONS];
	int mouse_motion_x, mouse_motion_y;
	bool mouse_wheel;
	int mouse_wheel_x, mouse_wheel_y;
};