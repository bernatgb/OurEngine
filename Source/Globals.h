#pragma once
#include <stdio.h>
#include <windows.h>

//Prepocesador: LOGGING_SUPPORT_DISABLED

#define MY_LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define DEGTORAD (math::pi / 180.0f)
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Super Awesome Engine"

#define GITHUB "https://github.com/miquelmiro3/MyEngine"
#define LICENSE_DIR "..\\LICENSE"
#define DEFAULT_SCREEN_RATIO 0.8

#define TEXTURES_FOLDER ".\\Assets\\Textures\\"
#define LIBRARY_MESHES ".\\Assets\\Library\\Meshes\\"
#define LIBRARY_MATERIALS ".\\Assets\\Library\\Materials\\"
#define LIBRARY_MODELS ".\\Assets\\Library\\Models\\"
#define RESOURCES_FILE ".\\Assets\\Library\\Resources.asset"
#define CONFIG_FILE ".\\Assets\\Library\\Config.asset"