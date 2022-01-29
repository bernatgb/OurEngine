#pragma once
#include "Module.h"

#include "Math/float2.h"
#include "Math/float4x4.h"
#include "Math/float3x3.h"
#include "Geometry/Frustum.h"

#include "CLight.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Material;
class CubeMap;

struct LightContainer {
public:
	int numberOfLights;
	int padding[3];
	Light lights[20];
};

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
	
	void DrawImGui();

	void ActivateMaterial(Material* _material);

	//Getters
	CubeMap* GetCubeMap();
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
	bool HasViewportSizeChanged() const
	{
		return viewportSizeChanged;
	};
	void* GetContext() 
	{
		return context; 
	};
	unsigned int GetProgram() const
	{
		return program; 
	};

private:
	void* context = nullptr;
	unsigned int program;

	unsigned int fbo;
	unsigned int fbo_texture;
	unsigned int rbo;
	unsigned int m_lightsUBO;

	LightContainer m_LightsContainer;

	float3 m_AmbientColor = float3(0.25f, 0.25f, 0.25f);

	CubeMap* cubeMap = nullptr;
	int m_ActiveCubeMap = -1;

	float2 viewportPanelSize;
	float2 sceneWindowPos;
	bool viewportSizeChanged;
};
