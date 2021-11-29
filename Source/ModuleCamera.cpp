#include "ModuleCamera.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderExercise.h"
#include "SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

void ModuleCamera::ViewProjectionMatrix()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(zNear, zFar);
	frustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);
	//TODO: setHoritzontal

	frustum.SetPos(eye);
	frustum.SetFront(rotationMatrix.WorldZ());
	frustum.SetUp(rotationMatrix.WorldY());
	/*frustum.SetPos(m_RotationMatrix.Col3(3));
	frustum.SetFront(m_RotationMatrix.WorldZ());
	frustum.SetUp(m_RotationMatrix.WorldY());*/

	view = float4x4(frustum.ViewMatrix());
	proj = frustum.ProjectionMatrix();
}

bool ModuleCamera::Init()
{
	eye = float3(0.0f, 3.0f, -8.0f);
	target = float3(0.0f, 0.0f, 0.0f);
	rotationMatrix = float3x3::FromEulerXYZ(DEGTORAD * 15.0f, 0.0f, 0.0f);
	m_RotationMatrix = float4x4::FromTRS(
		eye,
		float3x3::FromEulerXYZ(DEGTORAD * 15.0f, 0.0f, 0.0f),
		float3::one
	);

	//view = float4x4::LookAt(float3(0.0f, 4.0f, 8.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY, float3::unitY);
	//LookAt(camera forward(right), target dir, local up, world up)

	aspect = (float)App->window->width / (float)App->window->height;

	if (aspect >= 1.0f)
		verticalFov = DEGTORAD * initialVerticalFov;
	else
		verticalFov = math::Atan(math::Tan(DEGTORAD * initialVerticalFov) / aspect);

	ViewProjectionMatrix();

	return true;
}

update_status ModuleCamera::Update()
{
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

		if (App->input->GetKey(SDL_SCANCODE_LALT))
		{
			eye += rotationMatrix.WorldZ() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;
		}
		else
		{
			rotationMatrix = 
				float3x3::RotateAxisAngle(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) * 
				rotationMatrix * 
				float3x3::RotateAxisAngle(float3::unitX, deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD);
			
			/*m_RotationMatrix =
				float4x4::RotateAxisAngle(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
				m_RotationMatrix *
				float4x4::RotateAxisAngle(float3::unitX, deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD);*/

			float multiplier = 1.0f;
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
				multiplier = 3.0f;

			if (App->input->GetKey(SDL_SCANCODE_W))
				eye += rotationMatrix.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_S))
				eye -= rotationMatrix.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_A))
				eye += rotationMatrix.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_D))
				eye -= rotationMatrix.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_Q))
				eye += rotationMatrix.WorldY() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_E))
				eye -= rotationMatrix.WorldY() * Time::GetDeltaTime() * speed * multiplier;
		}

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		eye += rotationMatrix.WorldX() * deltaX * Time::GetDeltaTime() * mouseSpeedForMovement +
			rotationMatrix.WorldY() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		/*float3 euler = rotationMatrix.ToEulerXYZ();
		float4x4 newRotation = float4x4::FromTRS(eye,
			float4x4::FromEulerXYZ(euler.x, euler.y, euler.z),
			float3::one);

		newRotation =
			float4x4::RotateAxisAngle(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
			float4x4::RotateAxisAngle(float3::unitX, deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) * 
			newRotation;

		eye = newRotation.Col3(3);*/

		

		//eye += rotationMatrix.WorldX() * -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation +
		//	rotationMatrix.WorldY() * deltaY * Time::GetDeltaTime() * mouseSpeedForRotation;

		//eye = float3x3::RotateAxisAngle(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
		//	float3x3::RotateAxisAngle(float3::unitX, deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) * 
		//	eye;

		float4 target = App->rendererExercise->modelObj->GetCenter();

		float4 vector = float4(eye.x - target.x, eye.y - target.y, eye.z - target.z, 0);

		vector = 
			vector *
			float4x4::RotateAxisAngle(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
			float4x4::RotateAxisAngle(float3::unitX, deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD);

		eye = float3(target.x + vector.x, target.y + vector.y, target.z + vector.z);

		float3 front = (float3(target.x, target.y, target.z) - eye).Normalized();
		float3 right = Cross(float3::unitY, front).Normalized();
		float3 up = Cross(front, right).Normalized();

		rotationMatrix = float3x3(right, up, front);

		/*float4 target = App->rendererExercise->modelObj->GetCenter();
		float3 vector = eye - float3(target.x, target.y, target.z);

		float3 up_vector = rotationMatrix.Col(1).Normalized();
		float3 right_vector = rotationMatrix.Col(0).Normalized();

		float3x3 rotation_matrixX = float3x3::RotateAxisAngle(up_vector, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD);
		float3x3 rotation_matrixY = float3x3::RotateAxisAngle(right_vector, deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD);

		float4 newEye = rotation_matrixY * rotation_matrixX * float4(eye.x, eye.y, eye.z, 1.0f);

		eye = float3(transformation * glm::vec4(eye, 1));

		// translate back to the origin, rotate and translate back to the pivot location
		glm::mat4x4 transformation = glm::translate(rotation_center) * rotation_matrixY * rotation_matrixX * glm::translate(-rotation_center);*/

		ViewProjectionMatrix();
	}
	else if (App->input->MouseWheel()) 
	{
		int wheelX, wheelY;
		App->input->GetMouseWheel(wheelX, wheelY);

		eye += rotationMatrix.WorldZ() * wheelY * Time::GetDeltaTime() * mouseWheelSpeed;

		ViewProjectionMatrix();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F)) 
	{
		AdjustToModel(App->rendererExercise->modelObj);
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera::WindowResized(unsigned width, unsigned height)
{
	aspect = (float)width / (float)height;

	if (aspect >= 1.0f)
		verticalFov = DEGTORAD * initialVerticalFov;
	else
		verticalFov = math::Atan(math::Tan(DEGTORAD * initialVerticalFov) / aspect);

	ViewProjectionMatrix();
}

void ModuleCamera::AdjustToModel(Model* _model)
{
	float4 newPos = _model->GetCenter();
	eye = float3(newPos.x, newPos.y, newPos.z) - rotationMatrix.WorldZ().Normalized() * _model->GetRadius();

	ViewProjectionMatrix();
}

void ModuleCamera::DrawImGui()
{
	ImGui::Text("Variables");
	if (ImGui::DragFloat("Vertical FOV", &initialVerticalFov, 1.0f, 10.0f, 160.0f, "%.2f")) {
		if (aspect >= 1)
			verticalFov = DEGTORAD * initialVerticalFov;
		else
			verticalFov = math::Atan(math::Tan(DEGTORAD * initialVerticalFov) / aspect);
	}
	ImGui::DragFloat("Z-near", &zNear, 1.0f, 0.1f, 5.0f, "%.2f");
	ImGui::DragFloat("Z-far", &zFar, 5.0f, 6.0f, 400.0f, "%.2f");
	
	ImGui::Text("Vectors");
	ImGui::DragFloat3("Cam pos", &eye[0], 1.0f, -25.0f, 25.0f, "%.2f");
	ImGui::Separator();

	ViewProjectionMatrix();
}
