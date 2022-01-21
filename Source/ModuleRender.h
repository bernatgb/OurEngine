#pragma once
#include "Module.h"

#include "Math/float2.h"
#include "Math/float4x4.h"
#include "Math/float3x3.h"
#include "Geometry/Frustum.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Texture;
class CubeMap;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender() override;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	
	void ActivateTexture(Texture* _texture);

	CubeMap* GetCubeMap();

	void* context;
	unsigned int program;

	//Gets
	float2 GetSceneWindowSize() const
	{
		return viewportPanelSize;
	};
	float GetSceneWindowAspect() const
	{
		return viewportPanelSize.x / viewportPanelSize.y;
	};
	float2 GetSceneWindowStartPos() const
	{
		return sceneWindowPos;
	};

private:

	unsigned int fbo;
	unsigned int fbo_texture;
	unsigned int rbo;

	CubeMap* cubeMap;

	float2 viewportPanelSize;

	float2 sceneWindowPos;
};
