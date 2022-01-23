#include "ModuleCamera.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
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

	if (m_CullingCamera != nullptr)
	{
		gameCameraFrustum = *m_CullingCamera->GetCCameraFrustum();

		/*gameCameraFrustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
		gameCameraFrustum.SetViewPlaneDistances(m_CullingCamera->zNear, m_CullingCamera->zFar);
		gameCameraFrustum.SetVerticalFovAndAspectRatio(verticalFov, aspect);*/

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

	aspect = (float)App->window->GetWidth() / (float)App->window->GetHeight();

	if (aspect >= 1.0f)
		verticalFov = DEGTORAD * Config::m_Fov;
	else
		verticalFov = math::Atan(math::Tan(DEGTORAD * Config::m_Fov) / aspect);

	ViewProjectionMatrix();

	return true;
}

update_status ModuleCamera::Update()
{
	OPTICK_CATEGORY("ModuleCamera::Update", Optick::Category::Camera);

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			Config::m_CamPosition += Config::m_CamRotation.WorldZ() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;
		}
		else
		{
			float deltaPitch = deltaY * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD;
			float deltaYaw = -deltaX * Time::GetDeltaTime() * mouseSpeedForRotation * DEGTORAD;

			Rotate(deltaPitch, deltaYaw);

			float multiplier = 1.0f;
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT)
				multiplier = 3.0f;

			if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
				Config::m_CamPosition += Config::m_CamRotation.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
				Config::m_CamPosition -= Config::m_CamRotation.WorldZ() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
				Config::m_CamPosition += Config::m_CamRotation.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
				Config::m_CamPosition -= Config::m_CamRotation.WorldX() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
				Config::m_CamPosition += Config::m_CamRotation.WorldY() * Time::GetDeltaTime() * speed * multiplier;
			if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
				Config::m_CamPosition -= Config::m_CamRotation.WorldY() * Time::GetDeltaTime() * speed * multiplier;
		}

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		Config::m_CamPosition += Config::m_CamRotation.WorldX() * deltaX * Time::GetDeltaTime() * mouseSpeedForMovement +
			Config::m_CamRotation.WorldY() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;

		ViewProjectionMatrix();
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
	{
		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		float4 target = float4(0.0f, 0.0f, 0.0f, 1.0f);
		GameObject* selectedGO = App->scene->GetSelectedGO();
		if (selectedGO != nullptr)
			target = selectedGO->GetCenter();

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
	else if (App->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_REPEAT)
	{
		GameObject* selectedGO = App->scene->GetSelectedGO();
		if (selectedGO != nullptr)
			AdjustToGO(selectedGO);
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

void ModuleCamera::AdjustToGO(GameObject* _go)
{
	bool meshFounded = false;
	for (int i = 0; i < _go->m_Components.size(); ++i)
	{
		if (_go->m_Components[i]->m_Type == ComponentType::MESH)
		{
			meshFounded = true;
			CMesh* mesh = (CMesh*)_go->m_Components[i];
			float4 newPos = float4(mesh->GetMinPoint().x + (mesh->GetMaxPoint().x - mesh->GetMinPoint().x) / 2.0f, mesh->GetMinPoint().y + (mesh->GetMaxPoint().y - mesh->GetMinPoint().y) / 2.0f, mesh->GetMinPoint().z + (mesh->GetMaxPoint().z - mesh->GetMinPoint().z) / 2.0f, 1.0f);
			Config::m_CamPosition = float3(newPos.x, newPos.y, newPos.z) - Config::m_CamRotation.WorldZ().Normalized() * Distance3(float4(mesh->GetMaxPoint(), 0.0f), float4(mesh->GetMinPoint(), 0.0f));
		}
	}

	if (!meshFounded)
	{
		for (int i = 0; i < _go->m_Children.size(); ++i)
		{
			AdjustToGO(_go->m_Children[i]);
		}
	}

	//float4 newPos = _go->GetCenter();
	//Config::m_CamPosition = float3(newPos.x, newPos.y, newPos.z) - Config::m_CamRotation.WorldZ().Normalized() * _go->GetDiameter();

	// TODO: USE THE FOLLOWING FORMULA FOR THE DISTANCE
	//dist = height / 2 / Math.tan(Math.PI * fov / 360);

	ViewProjectionMatrix();
}

void ModuleCamera::SetCullingCamera(CCamera* _camera)
{
	if (_camera == nullptr) 
	{
		m_CullingCamera = nullptr;
	}
	else if (m_CullingCamera == nullptr)
	{
		m_CullingCamera = _camera;
	}
	else 
	{
		m_CullingCamera->SetCullingCamera(false);
		m_CullingCamera = _camera;
	}

	ViewProjectionMatrix();
}

Frustum* ModuleCamera::GetCullingCamera()
{
	if (m_CullingCamera == nullptr)
		return &frustum;
	else
		return &gameCameraFrustum;
}

Frustum* ModuleCamera::GetFrustum()
{
	return &frustum;
}
/*
Frustum* ModuleCamera::GetGameCameraFrustum()
{
	return &gameCameraFrustum;
}
*/
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

void ModuleCamera::FindIfRayIntersectsQuadtreeAreasAndAddGameObjectsToHits(LineSegment ray, QuadtreeNode* qtn, std::vector<GameObject*> &hits)
{
	if (ray.Intersects(qtn->GetAABB()))
	{
		std::list<GameObject*> gameObjects = qtn->GetGameObjectsInThisNode();
		for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		{
			GameObject* go = *it;
			if (ray.Intersects(go->m_aabb))
			{
				hits.push_back(go); 
				ImGui::Text("Ray intersects with %s", go->m_Name);
			}
		}

		if (qtn->GetChildren()[0] != nullptr) 
		{
			for (int i = 0; i < 4; ++i)
				FindIfRayIntersectsQuadtreeAreasAndAddGameObjectsToHits(ray, qtn->GetChildren()[i], hits);
		}
	}
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
	std::vector< std::pair<float, GameObject*> > hitsDistances(hits.size());
	for (int i = 0; i < hitsDistances.size(); ++i)
		hitsDistances[i] = std::pair<float, GameObject*>(hits[i]->m_aabb.Distance(frustum.Pos()), hits[i]);

	std::sort(hitsDistances.begin(), hitsDistances.end());

	for (int i = 0; i < hitsDistances.size(); ++i)
		hits[i] = hitsDistances[i].second;
}

void ModuleCamera::FindIfRayIntersectsATriangle(LineSegment ray, std::vector<GameObject*>& hits, std::vector< std::pair<float, GameObject*> >& hitPoints)
{
	for (int i = 0; i < hits.size(); ++i)
	{
		// Transform once the ray into Game Object space to test against all triangles
		float4x4 modelMatrix = hits[i]->m_Transform->m_AccumulativeModelMatrix;
		modelMatrix.Inverse();
		vec rayOrigin = (modelMatrix * float4(ray.a, 1.0f)).xyz();
		vec rayDir = ((modelMatrix * float4(ray.Dir().xyz(), 0.0f)).Normalized()).xyz();
		LineSegment rayLocalSpace = LineSegment(Ray(rayOrigin, rayDir), 500.0f);

		for (int j = 0; j < hits[i]->m_Components.size(); ++j)
		{
			if (hits[i]->m_Components[j]->m_Type == ComponentType::MESH)
			{
				CMesh* cMesh = (CMesh*)hits[i]->m_Components[j];
				std::vector<Triangle> triangles = cMesh->GetTriangles();
				
				for (int k = 0; k < triangles.size(); ++k)
				{
					Triangle triangle = triangles[k];
					float dist = 500.0f;
					vec hitPoint;
					if (rayLocalSpace.Intersects(triangles[k], &dist, &hitPoint))
						hitPoints.push_back(std::pair<float, GameObject*>(Distance(hitPoint, rayOrigin), hits[i]));
				}
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
		ImGui::DragFloat("Z-near", &Config::m_ZNear, 0.5f, 0.01f, 5.0f, "%.2f");
		ImGui::DragFloat("Z-far", &Config::m_ZFar, 5.0f, 6.0f, 1000.0f, "%.2f");
	
		ImGui::Text("Vectors");
		ImGui::DragFloat3("Cam pos", &Config::m_CamPosition[0], 1.0f, -25.0f, 25.0f, "%.2f");
		/// <summary>
		/// 
		/// </summary>
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
		{
			int mouse_x, mouse_y;
			App->input->GetMousePosition(mouse_x, mouse_y);
			ImGui::Text("x clicked = %i, y clicked = %i", mouse_x, mouse_y);

			ImVec2 v2 = ImGui::GetMousePos();
			ImGui::Text("mouse x = %i, mouse y = %i", (int)v2.x, (int)v2.y);
			
			const ImVec2 vContentRegionMin = ImGui::GetWindowContentRegionMin();
			//const ImVec2 vContentRegionMax = ImGui::GetWindowContentRegionMax();

			mouse_x -= App->renderer->GetSceneWindowStartPos().x + vContentRegionMin.x;
			mouse_y -= App->renderer->GetSceneWindowStartPos().y + vContentRegionMin.y;

			//ImGui::Text("scene starts at:");
			//ImGui::Text("x = %i, y = %i", (int)App->renderer->GetSceneWindowStartPos().x + 8, (int)App->renderer->GetSceneWindowStartPos().y + 37);

			float width = App->renderer->GetSceneWindowSize().x; 
			float height = App->renderer->GetSceneWindowSize().y; 

			//ImGui::Text("width = %i, height = %i", (int)width, (int)height);

			float x = (2.0f * mouse_x) / width - 1.0f;
			float y = 1.0f - (2.0f * mouse_y) / height;
			float z = 1.0f;
			vec ray_nds = vec(x, y, z);
			ImGui::Text("ray_nds = (%f, %f, %f)", x, y, z);

			float4 ray_clip = float4(ray_nds.xy(), -1.0, 1.0);
			
			float4 ray_eye = frustum.ProjectionMatrix().Inverse() * ray_clip;
			ray_eye = float4(ray_eye.xy(), -1.0f, 0.0f);
			
			float3 ray_world = float3((frustum.ViewMatrix().Inverse() * ray_eye).xyz());
			ray_world = (ray_world - frustum.Pos()).Normalized();
			ImGui::Text("ray_world = (%f, %f, %f)", ray_world.x, ray_world.y, ray_world.z);

			LineSegment ray = frustum.UnProjectLineSegment(x, y);
			std::vector<GameObject*> hits;
			/*
			const GameObject* root = App->scene->GetRoot();
			for (int i = 0; i < root->m_Children.size(); ++i)
				FindIfRayIntersectsAnAABBandAddToHits(ray, root->m_Children[i], hits);
			*/
			QuadtreeNode* qtRoot = App->scene->GetQuadtree()->GetRoot();
			FindIfRayIntersectsQuadtreeAreasAndAddGameObjectsToHits(ray, qtRoot, hits);

			if (!hits.empty())
			{
				SortHits(hits); // With ray.length?
				//ImGui::Text("Mouse is pointing nt %s", hits[4]->m_Name);
				std::vector< std::pair<float, GameObject*> > hitPointsDistances;
				FindIfRayIntersectsATriangle(ray, hits, hitPointsDistances);
				std::sort(hitPointsDistances.begin(), hitPointsDistances.end());
				if (!hitPointsDistances.empty()) 
					ImGui::Text("Mouse is pointing %s", hitPointsDistances[0].second->m_Name);
			}
		}
		
		ViewProjectionMatrix();
	}
}
