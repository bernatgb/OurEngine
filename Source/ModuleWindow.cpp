#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	MY_LOG("Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		MY_LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = SCREEN_WIDTH;
		height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if (FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			fullscreen = true;
		}
		else
		{
			flags |= SDL_WINDOW_RESIZABLE;
			fullscreen = false;
		}

		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		width = current.w * DEFAULT_SCREEN_RATIO;
		height = current.h * DEFAULT_SCREEN_RATIO;

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL)
		{
			MY_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface

			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	MY_LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::WindowResized(unsigned _width, unsigned _height)
{
	width = _width;
	height = _height;
}

void ModuleWindow::DrawImGui()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
			if (fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else SDL_SetWindowFullscreen(window, 0);
		}

		ImGui::Separator();

		ImGui::Text("Width: %i", width);
		ImGui::Text("Height: %i", height);
		
		ImGui::Separator();

		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		ImGui::Text("Screen width: %i", current.w);
		ImGui::Text("Screen height: %i", current.h);
	}
}

