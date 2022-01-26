#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleResources.h"
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
    
    memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
    memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);

    keys = SDL_GetKeyboardState(NULL);

	return ret;
}

update_status ModuleInput::PreUpdate()
{
    OPTICK_CATEGORY("ModuleInput::PreUpdate", Optick::Category::Input);

    //SDL_PumpEvents();

    SDL_Event sdlEvent;

    mouse_motion_x = mouse_motion_y = 0;
    mouse_wheel = false;
    mouse_wheel_x = mouse_wheel_y = 0;

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys[i] == 1)
        {
            if (keyboard[i] == KEY_IDLE)
                keyboard[i] = KEY_DOWN;
            else
                keyboard[i] = KEY_REPEAT;
        }
        else
        {
            if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
                keyboard[i] = KEY_UP;
            else
                keyboard[i] = KEY_IDLE;
        }
    }

    for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
    {
        if (mouse_buttons[i] == KEY_DOWN)
            mouse_buttons[i] = KEY_REPEAT;

        if (mouse_buttons[i] == KEY_UP)
            mouse_buttons[i] = KEY_IDLE;
    }

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
                mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
                break;
            case SDL_MOUSEMOTION:
                mouse_position_x = sdlEvent.button.x;
                mouse_position_y = sdlEvent.button.y;
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
                App->resources->LoadResource(sdlEvent.drop.file);
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