#pragma once
#include "Module.h"

#include "Math/float4x4.h"

class DDRenderInterfaceCoreGL;
class Camera;

class ModuleDebugDraw : public Module
{
public:
    ModuleDebugDraw();
    ~ModuleDebugDraw() override;

	bool Init();
	update_status Update();
	bool CleanUp();

    void DrawBB(const float3* vertex);
    void Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);
    void DrawRay(float3 from, float3 to);
    void DrawDirectionalLight(float3 from, float3 to);
    void DrawPointLight(float3 center, float radius);
    void DrawSpotLight(float3 apex, float3 direction, float baseRadius, float apexRadius, float3 color);

private:
    static DDRenderInterfaceCoreGL* implementation;
};
