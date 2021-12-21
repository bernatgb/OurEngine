#include "ModuleCamera.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "SDL.h"

#include "CCamera.h"

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
	if (m_CurrentCamera == nullptr) 
	{
		frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
		frustum.SetViewPlaneDistances(zNear, zFar);
		frustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);
		//TODO: setHoritzontal

		frustum.SetPos(eye);
		frustum.SetFront(m_CameraRotation.WorldZ());
		frustum.SetUp(m_CameraRotation.WorldY());
		/*frustum.SetPos(m_RotationMatrix.Col3(3));
		frustum.SetFront(m_RotationMatrix.WorldZ());
		frustum.SetUp(m_RotationMatrix.WorldY());*/
	}
	else 
	{
		frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
		frustum.SetViewPlaneDistances(zNear, zFar);
		frustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);

		//frustum.SetPos(m_CurrentCamera->m_Owner->m_Transform->GetPos());
		//frustum.SetFront(m_CurrentCamera->m_Owner->m_Transform->GetForward());
		//frustum.SetUp(m_CurrentCamera->m_Owner->m_Transform->GetUp());

		frustum.SetPos(m_CurrentCamera->pos);
		frustum.SetFront(m_CurrentCamera->front);
		frustum.SetUp(m_CurrentCamera->up);
	}

	view = float4x4(frustum.ViewMatrix());
	proj = frustum.ProjectionMatrix();
}

void ModuleCamera::Rotate(float _deltaPitch, float _deltaYaw)
{
	m_PitchYawRoll.x += _deltaPitch;
	m_PitchYawRoll.y += _deltaYaw;

	m_CameraRotation =
		Quat::RotateAxisAngle(float3::unitY, _deltaYaw) *
		m_CameraRotation *
		Quat::RotateAxisAngle(float3::unitX, _deltaPitch);
}

bool ModuleCamera::Init()
{
	eye = float3(0.0f, 3.0f, -8.0f);
	m_PitchYawRoll = float3(DEGTORAD * 15.0f, 0.0f, 0.0f);
	m_CameraRotation = float3x3::FromEulerXYZ(DEGTORAD * 15.0f, 0.0f, 0.0f).ToQuat();

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
	if (m_CurrentCamera != nullptr)
		return UPDATE_CONTINUE;

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
			eye += m_CameraRotation.WorldZ() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;
		}
		else
		{
			float deltaPitch = deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD;
			float deltaYaw = -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD;

			Rotate(deltaPitch, deltaYaw);

			float multiplier = 1.0f;
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
				multiplier = 3.0f;

			if (App->input->GetKey(SDL_SCANCODE_W))
				eye += m_CameraRotation.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_S))
				eye -= m_CameraRotation.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_A))
				eye += m_CameraRotation.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_D))
				eye -= m_CameraRotation.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_Q))
				eye += m_CameraRotation.WorldY() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_E))
				eye -= m_CameraRotation.WorldY() * Time::GetDeltaTime() * speed * multiplier;
		}

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		eye += m_CameraRotation.WorldX() * deltaX * Time::GetDeltaTime() * mouseSpeedForMovement +
			m_CameraRotation.WorldY() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		float4 target = App->scene->models[App->scene->activeModel]->GetCenter();

		float4 vector = float4(eye.x - target.x, eye.y - target.y, eye.z - target.z, 0);

		vector = 
			Quat(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
			Quat(m_CameraRotation.WorldX(), deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
			vector;

		/*float aux = Dot(vector, float4::unitY);
		if (aux > -0.98f && aux < 0.98f)
		{
		}*/

		eye = float3(target.x + vector.x, target.y + vector.y, target.z + vector.z);

		float3 front = (float3(target.x, target.y, target.z) - eye).Normalized();
		float3 right = Cross(float3::unitY, front).Normalized();
		float3 up = Cross(front, right).Normalized();

		m_CameraRotation = float3x3(right, up, front).ToQuat();

		ViewProjectionMatrix();
	}
	else if (App->input->MouseWheel()) 
	{
		int wheelX, wheelY;
		App->input->GetMouseWheel(wheelX, wheelY);

		eye += m_CameraRotation.WorldZ() * wheelY * Time::GetDeltaTime() * mouseWheelSpeed;

		ViewProjectionMatrix();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F)) 
	{
		AdjustToModel(App->scene->models[App->scene->activeModel]);
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
	eye = float3(newPos.x, newPos.y, newPos.z) - m_CameraRotation.WorldZ().Normalized() * _model->GetDiameter();

	//dist = height / 2 / Math.tan(Math.PI * fov / 360);

	ViewProjectionMatrix();
}

void ModuleCamera::SetCurrentCamera(CCamera* _camera)
{
	if (_camera == nullptr) 
	{
		m_CurrentCamera = nullptr;
	}
	else if (m_CurrentCamera == nullptr)
	{
		m_CurrentCamera = _camera;
	}
	else 
	{
		m_CurrentCamera->m_CurrentCamera = false;
		m_CurrentCamera = _camera;
	}

	ViewProjectionMatrix();
}

Frustum* ModuleCamera::GetFrustum()
{
	return &frustum;
}

// false if fully outside, true if inside or intersects
bool ModuleCamera::BoxInFrustum(Frustum const& fru, const float3* box)
{
	// check box outside/inside of frustum
	for (int i = 0; i < 6; i++)
	{
		int out = 0;

		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[0]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[1]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[2]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[3]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[4]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[5]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[6]));
		out += fru.GetPlane(i).IsOnPositiveSide(vec(box[7]));

		if (out == 8) return false;
	}

	// check frustum outside/inside box
	int out;

	float3* fPoints = new float3[8];
	fru.GetCornerPoints(fPoints);

	// box[0] = all max components
	// box[6] = all min components
	
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].x > box[0].x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].x < box[6].x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].y > box[0].y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].y < box[6].y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].z > box[0].z) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].z < box[6].z) ? 1 : 0); if (out == 8) return false;
	
	return true;
}


void ModuleCamera::DrawImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
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

		ViewProjectionMatrix();
	}
}
