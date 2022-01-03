#include "ModuleCamera.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "SDL.h"

#include "CCamera.h"
#include "Config.h"

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
	frustum.SetViewPlaneDistances(Config::m_ZNear, Config::m_ZFar);
	frustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);
	//TODO: setHoritzontal

	frustum.SetPos(Config::m_CamPosition);
	frustum.SetFront(Config::m_CamRotation.WorldZ());
	frustum.SetUp(Config::m_CamRotation.WorldY());

	if (m_CurrentCamera != nullptr) 
	{
		gameCameraFrustum = m_CurrentCamera->frustum;

		gameCameraFrustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
		gameCameraFrustum.SetViewPlaneDistances(m_CurrentCamera->zNear, m_CurrentCamera->zFar);
		gameCameraFrustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);

		// To visualize what gameCamera sees
		//frustum = gameCameraFrustum;
	}	 

	view = float4x4(frustum.ViewMatrix());
	proj = frustum.ProjectionMatrix();
}

void ModuleCamera::Rotate(float _deltaPitch, float _deltaYaw)
{
	m_PitchYawRoll.x += _deltaPitch;
	m_PitchYawRoll.y += _deltaYaw;

	Config::m_CamRotation =
		Quat::RotateAxisAngle(float3::unitY, _deltaYaw) *
		Config::m_CamRotation *
		Quat::RotateAxisAngle(float3::unitX, _deltaPitch);
}

bool ModuleCamera::Init()
{
	//Config::m_CamPosition = float3(0.0f, 3.0f, -8.0f);
	m_PitchYawRoll = float3(DEGTORAD * 15.0f, 0.0f, 0.0f);
	//Config::m_CamRotation = float3x3::FromEulerXYZ(DEGTORAD * 15.0f, 0.0f, 0.0f).ToQuat();

	//view = float4x4::LookAt(float3(0.0f, 4.0f, 8.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY, float3::unitY);
	//LookAt(camera forward(right), target dir, local up, world up)

	aspect = (float)App->window->width / (float)App->window->height;

	if (aspect >= 1.0f)
		verticalFov = DEGTORAD * Config::m_Fov;
	else
		verticalFov = math::Atan(math::Tan(DEGTORAD * Config::m_Fov) / aspect);

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
			Config::m_CamPosition += Config::m_CamRotation.WorldZ() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;
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
				Config::m_CamPosition += Config::m_CamRotation.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_S))
				Config::m_CamPosition -= Config::m_CamRotation.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_A))
				Config::m_CamPosition += Config::m_CamRotation.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_D))
				Config::m_CamPosition -= Config::m_CamRotation.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_Q))
				Config::m_CamPosition += Config::m_CamRotation.WorldY() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_E))
				Config::m_CamPosition -= Config::m_CamRotation.WorldY() * Time::GetDeltaTime() * speed * multiplier;
		}

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		Config::m_CamPosition += Config::m_CamRotation.WorldX() * deltaX * Time::GetDeltaTime() * mouseSpeedForMovement +
			Config::m_CamRotation.WorldY() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) && App->input->GetKey(SDL_SCANCODE_LALT))
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		// TODO: FIX THIS
		//float4 target = App->scene->models[App->scene->activeModel]->GetCenter();
		float4 target = float4(0.0f, 0.0f, 0.0f, 1.0f);

		float4 vector = float4(Config::m_CamPosition.x - target.x, Config::m_CamPosition.y - target.y, Config::m_CamPosition.z - target.z, 0);

		vector = 
			Quat(float3::unitY, -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
			Quat(Config::m_CamRotation.WorldX(), deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD) *
			vector;

		/*float aux = Dot(vector, float4::unitY);
		if (aux > -0.98f && aux < 0.98f)
		{
		}*/

		Config::m_CamPosition = float3(target.x + vector.x, target.y + vector.y, target.z + vector.z);

		float3 front = (float3(target.x, target.y, target.z) - Config::m_CamPosition).Normalized();
		float3 right = Cross(float3::unitY, front).Normalized();
		float3 up = Cross(front, right).Normalized();

		Config::m_CamRotation = float3x3(right, up, front).ToQuat();

		ViewProjectionMatrix();
	}
	else if (App->input->MouseWheel()) 
	{
		int wheelX, wheelY;
		App->input->GetMouseWheel(wheelX, wheelY);

		Config::m_CamPosition += Config::m_CamRotation.WorldZ() * wheelY * Time::GetDeltaTime() * mouseWheelSpeed;

		ViewProjectionMatrix();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F)) 
	{
		// TODO: FIX THIS
		//AdjustToModel(App->scene->models[App->scene->activeModel]);
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera::WindowResized(unsigned width, unsigned height)
{
	aspect = (float)width / (float)height;

	if (aspect >= 1.0f)
		verticalFov = DEGTORAD * Config::m_Fov;
	else
		verticalFov = math::Atan(math::Tan(DEGTORAD * Config::m_Fov) / aspect);

	ViewProjectionMatrix();
}

void ModuleCamera::AdjustToModel(Model* _model)
{
	float4 newPos = _model->GetCenter();
	Config::m_CamPosition = float3(newPos.x, newPos.y, newPos.z) - Config::m_CamRotation.WorldZ().Normalized() * _model->GetDiameter();

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

Frustum* ModuleCamera::GetGameCameraFrustum()
{
	return &gameCameraFrustum;
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
	
	delete fPoints;

	return true;
}

void ModuleCamera::FindIfRayIntersectsAnAABBandAddToHits(LineSegment ray, GameObject* go, std::vector<GameObject*> &hits)
{
	if (ray.Intersects(go->m_aabb))
	{
		ImGui::Text("Ray intersects with %s", go->m_Name); 
		hits.push_back(go);
	}
	for (int i = 0; i < go->m_Children.size(); ++i)
		FindIfRayIntersectsAnAABBandAddToHits(ray, go->m_Children[i], hits);
}

void ModuleCamera::SortHits(std::vector<GameObject*>& hits)
{
	/* With ints
	std::vector <std::pair<float, int> > hitsDistances(hits.size());
	for (int i = 0; i < hitsDistances.size(); ++i)
		hitsDistances[i] = std::pair<float, int> (hits[i]->m_aabb.Distance(frustum.Pos()), i);

	std::sort(hitsDistances.begin(), hitsDistances.end());

	std::vector<GameObject*> hitsCopy = hits;
	for (int i = 0; i < hitsDistances.size(); ++i)
		hits[i] = hitsCopy[hitsDistances[i].second];
	*/

	/* With GameObjects */
	std::vector <std::pair<float, GameObject*> > hitsDistances(hits.size());
	for (int i = 0; i < hitsDistances.size(); ++i)
		hitsDistances[i] = std::pair<float, GameObject*>(hits[i]->m_aabb.Distance(frustum.Pos()), hits[i]);

	std::sort(hitsDistances.begin(), hitsDistances.end());

	for (int i = 0; i < hitsDistances.size(); ++i)
		hits[i] = hitsDistances[i].second;
}

void ModuleCamera::FindIfRayIntersectsATriangle(LineSegment ray, std::vector<GameObject*>& hits)
{
	for (int i = 0; i < hits.size(); ++i)
	{
		for (int j = 0; j < hits[i]->m_Components.size(); ++j)
		{
			if (hits[i]->m_Components[i]->m_Type == ComponentType::MESH)
			{
				CMesh* cMesh = (CMesh*)hits[i]->m_Components[i];
				//math::Triangle tri = (Triangle)cMesh->m_Triangles[];
				//bool hit = ray_local_space.Intersects(tri, &distance, &hit_point);
			}
		}
	}
}

void ModuleCamera::DrawImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("Variables");
		if (ImGui::DragFloat("Vertical FOV", &Config::m_Fov, 1.0f, 10.0f, 160.0f, "%.2f")) {
			if (aspect >= 1)
				verticalFov = DEGTORAD * Config::m_Fov;
			else
				verticalFov = math::Atan(math::Tan(DEGTORAD * Config::m_Fov) / aspect);
		}
		ImGui::DragFloat("Z-near", &Config::m_ZNear, 1.0f, 0.1f, 5.0f, "%.2f");
		ImGui::DragFloat("Z-far", &Config::m_ZNear, 5.0f, 6.0f, 400.0f, "%.2f");
	
		ImGui::Text("Vectors");
		ImGui::DragFloat3("Cam pos", &Config::m_CamPosition[0], 1.0f, -25.0f, 25.0f, "%.2f");
		
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT))
		{
			int mouse_x, mouse_y;
			App->input->GetMousePosition(mouse_x, mouse_y);
			ImGui::Text("x=%i, y=%i", mouse_x, mouse_y);
			// x = 280, y = 45	|	x = 870, y = 45
			// x = 280, y = 530	|	x = 870, y = 530
			// When it's not in fullscreen (in my PC works XD)
			
			mouse_x -= 280;
			mouse_y -= 45;
			float width = 870 - 280; // App->render->viewportPanelSize.x
			float height = 530 - 45; // App->render->viewportPanelSize.y
			float x = (2.0f * mouse_x) / width - 1.0f;
			float y = 1.0f - (2.0f * mouse_y) / height;
			float z = 1.0f;
			vec ray_nds = vec(x, y, z);
			ImGui::Text("ray_nds = (%f, %f, %f)", x, y, z);
			float4 ray_clip = float4(ray_nds.xy(), -1.0, 1.0);
			float4 ray_eye = frustum.ProjectionMatrix().Inverse() * ray_clip;
			ray_eye = float4(ray_eye.xy(), -1.0f, 0.0f);
			float3 ray_world = float3((frustum.ViewMatrix().Inverse() * ray_eye).xyz());
			ray_world = ray_world.Normalized();
			ImGui::Text("ray_world = (%f, %f, %f)", ray_world.x, ray_world.y, ray_world.z);

			LineSegment ray = frustum.UnProjectLineSegment(x, y);
			const GameObject* root = App->scene->GetRoot();
			std::vector<GameObject*> hits;
			for (int i = 0; i < root->m_Children.size(); ++i)
				FindIfRayIntersectsAnAABBandAddToHits(ray, root->m_Children[i], hits);
			if (!hits.empty())
			{
				SortHits(hits); // With ray.length?
				FindIfRayIntersectsATriangle(ray, hits);
			}
		}
		
		ViewProjectionMatrix();
	}
}
