#include "ModuleCamera.h"

#include "Application.h"
#include "ModuleInput.h"
#include "SDL.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

void ModuleCamera::ViewProjectionMatrix()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetVerticalFovAndAspectRatio(DEGTORAD * 45.0f, aspect);

	frustum.SetPos(eye);
	frustum.SetFront(rotationMatrix.WorldZ());
	frustum.SetUp(rotationMatrix.WorldY());

	view = float4x4(frustum.ViewMatrix());
	proj = frustum.ProjectionMatrix();
}

bool ModuleCamera::Init()
{
	eye = float3(0.0f, 4.0f, 8.0f);
	target = float3(0.0f, 0.0f, 0.0f);
	rotationMatrix = float3x3::FromEulerXYZ(DEGTORAD * -30.0f, DEGTORAD * 180.0f, 0.0f);

	//view = ViewMatrix(target, eye);
	//view = float4x4::LookAt(float3(0.0f, 4.0f, 8.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY, float3::unitY);

	aspect = SCREEN_WIDTH / SCREEN_HEIGHT;

	ViewProjectionMatrix();

	return true;
}

update_status ModuleCamera::Update()
{
	bool change = false;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		/*if (App->input->GetKey(SDL_SCANCODE_W) || App->input->GetKey(SDL_SCANCODE_S) ||
			App->input->GetKey(SDL_SCANCODE_A) || App->input->GetKey(SDL_SCANCODE_D) ||
			App->input->GetKey(SDL_SCANCODE_Q) || App->input->GetKey(SDL_SCANCODE_E))
		{
			deltaX *= -1;
			deltaY *= -1;
		}*/

		float3x3 rotationDeltaMatrix =
			float3x3::RotateAxisAngle(rotationMatrix.WorldX(), deltaY * 0.25f * DEGTORAD) *
			float3x3::RotateAxisAngle(float3(0.0f, 1.0f, 0.0f), -deltaX * 0.25f * DEGTORAD);

		rotationMatrix = rotationDeltaMatrix * rotationMatrix;

		MY_LOG("%i %i", deltaX, deltaY);

		if (App->input->GetKey(SDL_SCANCODE_W))
			eye += rotationMatrix.WorldZ() * 0.1f;
		if (App->input->GetKey(SDL_SCANCODE_S))
			eye += rotationMatrix.WorldZ() * -0.1f;
		if (App->input->GetKey(SDL_SCANCODE_A))
			eye += rotationMatrix.WorldX() * 0.1f;
		if (App->input->GetKey(SDL_SCANCODE_D))
			eye += rotationMatrix.WorldX() * -0.1f;
		if (App->input->GetKey(SDL_SCANCODE_Q))
			eye += rotationMatrix.WorldY() * 0.1f;
		if (App->input->GetKey(SDL_SCANCODE_E))
			eye += rotationMatrix.WorldY() * -0.1f;

		change = true;
	}
	else
	{
		if (App->input->GetKey(SDL_SCANCODE_W))
		{
			eye += float3(0.0f, 0.0f, -0.1f);
			change = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S))
		{
			eye += float3(0.0f, 0.0f, 0.1f);
			change = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_A))
		{
			eye += float3(-0.1f, 0.0f, 0.0f);
			change = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_D))
		{
			eye += float3(0.1f, 0.0f, 0.0f);
			change = true;
		}
	}

	if (change)
		ViewProjectionMatrix();

	return UPDATE_CONTINUE;
}

void ModuleCamera::WindowResized(unsigned width, unsigned height)
{
	aspect = width / height;

	ViewProjectionMatrix();
}