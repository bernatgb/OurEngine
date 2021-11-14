#pragma once
#include "Module.h"

#include "SDL/include/SDL.h"

typedef unsigned __int8 Uint8;

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status Update();
	bool CleanUp();

	const Uint8 GetKey(SDL_Scancode key) const;

private:
	const Uint8 *keyboard = NULL;
};