#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "imgui_impl_sdl.h"

ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{
	MY_LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		MY_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
    
    keyboard = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < NUM_MOUSE_BUTTONS; i++)
        mouse_buttons[i] = false;

	return ret;
}

update_status ModuleInput::PreUpdate()
{
    //SDL_PumpEvents();

    SDL_Event sdlEvent;

    mouse_motion_x = mouse_motion_y = 0;
    mouse_wheel = false;
    mouse_wheel_x = mouse_wheel_y = 0;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        //ImGui::IsItemHovered
        //ImGuiIO& io = ImGui::GetIO();
        
        //if (io.WantCaptureMouse)
        //    break;

        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;
            case SDL_WINDOWEVENT:
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) 
                {
                    App->window->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                    App->camera->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                    App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:     
                mouse_buttons[sdlEvent.button.button - 1] = true;
                mouse_position_x = sdlEvent.button.x;
                mouse_position_y = sdlEvent.button.y;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_buttons[sdlEvent.button.button - 1] = false;
                break;
            case SDL_MOUSEMOTION:
                mouse_motion_x = sdlEvent.motion.xrel;
                mouse_motion_y = sdlEvent.motion.yrel;
                break;
            case SDL_MOUSEWHEEL:
                mouse_wheel = true;
                mouse_wheel_x = sdlEvent.wheel.x;
                mouse_wheel_y = sdlEvent.wheel.y;
                break;
            case SDL_DROPFILE: {
                MY_LOG("File dropped %s", sdlEvent.drop.file);
                App->scene->LoadModel(sdlEvent.drop.file);
                SDL_free(sdlEvent.drop.file);
                break;
            }
        }
    }

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	MY_LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

const Uint8 ModuleInput::GetKey(SDL_Scancode key) const
{
    return keyboard[key];
}

const bool ModuleInput::GetMouseButton(unsigned int key) const
{
    return mouse_buttons[key - 1];
}

const void ModuleInput::GetMouseMotion(int& x, int& y) const
{
    x = mouse_motion_x;
    y = mouse_motion_y;
}
