#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow() override;

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	void WindowResized(unsigned _width, unsigned _height);
	void DrawImGui();

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;

	int width, height;

	bool fullscreen;
};

#endif // __ModuleWindow_H__