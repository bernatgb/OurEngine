#pragma once
#include "Module.h"

#include "SDL/include/SDL.h"

typedef unsigned __int8 Uint8;

#define MAX_KEYS 300
#define NUM_MOUSE_BUTTONS 5

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput() override;

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	KeyState GetKey(int id) const { return keyboard[id]; }
	KeyState GetMouseButton(int id) const { return mouse_buttons[id - 1]; }

	void GetMouseMotion(int& x, int& y) const { x = mouse_motion_x; y = mouse_motion_y; };
	void GetMousePosition(int& x, int& y) const { x = mouse_position_x; y = mouse_position_y; };

	const bool MouseWheel() const { return mouse_wheel; };
	void GetMouseWheel(int& x, int& y) const { x = mouse_wheel_x; y = mouse_wheel_y; };

private:
	KeyState keyboard[MAX_KEYS];
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];

	const Uint8 *keys = NULL;
	int mouse_motion_x, mouse_motion_y;
	int mouse_position_x, mouse_position_y;
	bool mouse_wheel;
	int mouse_wheel_x, mouse_wheel_y;
};
