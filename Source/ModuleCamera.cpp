#include "ModuleCamera.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
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

	ImVec2 pos = ImVec2(
		App->renderer->GetSceneWindowStartPos().x + ImGui::GetWindowContentRegionMin().x + App->renderer->GetSceneWindowSize().x - 100.0f,
		App->renderer->GetSceneWindowStartPos().y + ImGui::GetWindowContentRegionMin().y
	);
	float4x4 auxView = view.Transposed();
	ImGuizmo::ViewManipulate(&auxView[0][0], 20.0f, pos, ImVec2(100.0f, 100.0f), 0x020202);
	view = auxView.Transposed();

	/*if (!auxView.Equals(view.Transposed(), 0.0001f))
	{
		int asdas = 0;
		//Config::m_CamPosition = auxView.Row3(3);
		Config::m_CamRotation = Quat(auxView);
		ViewProjectionMatrix();
		return UPDATE_CONTINUE;
	}*/

	if (App->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_REPEAT)
	{
		GameObject* selectedGO = App->scene->GetSelectedGO();
		if (selectedGO != nullptr)
			AdjustToGO(selectedGO);
	}

	if (IsMouseInScene())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			m_CameraRotation = true;
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
		{
			m_CameraScrolling = true;
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			m_CameraZooming = true;
		}
		else if (App->input->MouseWheel())
		{
			int wheelX, wheelY;
			App->input->GetMouseWheel(wheelX, wheelY);

			Config::m_CamPosition += Config::m_CamRotation.WorldZ() * wheelY * Time::GetDeltaTime() * mouseWheelSpeed;

			ViewProjectionMatrix();
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && !ImGuizmo::IsOver())
		{
			int mouse_x, mouse_y;
			App->input->GetMousePosition(mouse_x, mouse_y);

			const ImVec2 vContentRegionMin = ImGui::GetWindowContentRegionMin();

			mouse_x -= App->renderer->GetSceneWindowStartPos().x + vContentRegionMin.x;
			mouse_y -= App->renderer->GetSceneWindowStartPos().y + vContentRegionMin.y;

			float width = App->renderer->GetSceneWindowSize().x;
			float height = App->renderer->GetSceneWindowSize().y;

			float x = (2.0f * mouse_x) / width - 1.0f;
			float y = 1.0f - (2.0f * mouse_y) / height;
			float z = 1.0f;
			vec ray_nds = vec(x, y, z);

			float4 ray_clip = float4(ray_nds.xy(), -1.0, 1.0);

			float4 ray_eye = frustum.ProjectionMatrix().Inverse() * ray_clip;
			ray_eye = float4(ray_eye.xy(), -1.0f, 0.0f);

			float3 ray_world = float3((frustum.ViewMatrix().Inverse() * ray_eye).xyz());
			ray_world = (ray_world - frustum.Pos()).Normalized();

			LineSegment ray = frustum.UnProjectLineSegment(x, y);

			std::vector<GameObject*> hits;
			QuadtreeNode* qtRoot = App->scene->GetQuadtree()->GetRoot();
			FindIfRayIntersectsQuadtreeAreasAndAddGameObjectsToHits(ray, qtRoot, hits);

			if (!hits.empty())
			{
				SortHits(hits);
				std::vector< std::pair<float, GameObject*> > hitPointsDistances;
				FindIfRayIntersectsATriangle(ray, hits, hitPointsDistances);
				std::sort(hitPointsDistances.begin(), hitPointsDistances.end());
				if (!hitPointsDistances.empty())
				{
					if (App->scene->GetSelectedGO() != hitPointsDistances[0].second)
						App->scene->SelectGameObject(hitPointsDistances[0].second);
					else
						App->scene->SelectGameObject(hitPointsDistances[0].second->m_Parent);
				}
				else
					App->scene->SelectGameObject(App->scene->GetSelectedGO());
			}
		}
	}

	if (m_CameraRotation)
	{
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_UP)
			m_CameraRotation = false;

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
	else if (m_CameraScrolling)
	{
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KeyState::KEY_UP)
			m_CameraScrolling = false;

		int deltaX, deltaY;
		App->input->GetMouseMotion(deltaX, deltaY);

		Config::m_CamPosition += Config::m_CamRotation.WorldX() * deltaX * Time::GetDeltaTime() * mouseSpeedForMovement +
			Config::m_CamRotation.WorldY() * deltaY * Time::GetDeltaTime() * mouseSpeedForMovement;

		ViewProjectionMatrix();
	}
	else if (m_CameraZooming)
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_UP || App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_UP)
			m_CameraZooming = false;

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

		Config::m_CamPosition = float3(target.x + vector.x, target.y + vector.y, target.z + vector.z);

		float3 front = (float3(target.x, target.y, target.z) - Config::m_CamPosition).Normalized();
		float3 right = Cross(float3::unitY, front).Normalized();
		float3 up = Cross(front, right).Normalized();

		Config::m_CamRotation = float3x3(right, up, front).ToQuat();

		ViewProjectionMatrix();
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

GameObject* RecursiveSearchBiggestGO(GameObject* _go) 
{
	GameObject* posibleResult = _go;
	float diameter = 0;
	if (!_go->m_Min.IsZero() || !_go->m_Max.IsZero())
		diameter = _go->GetDiameter();

	for (int i = 0; i < _go->m_Children.size(); ++i)
	{
		GameObject* aux = RecursiveSearchBiggestGO(_go->m_Children[i]);

		if (!aux->m_Min.IsZero() || !aux->m_Max.IsZero()) 
		{
			float newDiameter = aux->GetDiameter();
			if (newDiameter > diameter)
			{
				diameter = newDiameter;
				posibleResult = aux;
			}
		}
	}
	return posibleResult;
}

void ModuleCamera::AdjustToGO(GameObject* _go)
{
	GameObject* gameObject = RecursiveSearchBiggestGO(_go);

	float4 newPos = gameObject->GetCenter();
	Config::m_CamPosition = float3(newPos.x, newPos.y, newPos.z) - Config::m_CamRotation.WorldZ().Normalized() * gameObject->GetDiameter();

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

	// box[3] = all max components
	// box[5] = all min components
	
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].x > box[3].x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].x < box[5].x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].y > box[3].y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].y < box[5].y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].z > box[3].z) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((fPoints[i].z < box[5].z) ? 1 : 0); if (out == 8) return false;
	
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
		LineSegment rayLocalSpace = ray;
		rayLocalSpace.Transform(modelMatrix);

		// For debug
		rayFrom = (hits[i]->m_Transform->m_AccumulativeModelMatrix * rayLocalSpace).a;
		rayTo = (hits[i]->m_Transform->m_AccumulativeModelMatrix * rayLocalSpace).b;


		for (int j = 0; j < hits[i]->m_Components.size(); ++j)
		{
			if (hits[i]->m_Components[j]->m_Type == ComponentType::MESH)
			{
				CMesh* cMesh = (CMesh*)hits[i]->m_Components[j];
				std::vector<Triangle> triangles = cMesh->GetTriangles();
				float dist = 500.0f;
				
				for (int k = 0; k < triangles.size(); ++k)
				{
					Triangle triangle = triangles[k];
					vec hitPoint;
					if (rayLocalSpace.Intersects(triangles[k], &dist, &hitPoint))
					{
						hitPoints.push_back(std::pair<float, GameObject*>(Distance(hitPoint, rayLocalSpace.a), hits[i]));
					}
				}
			}
		}
	}
}

bool ModuleCamera::IsMouseInScene()
{
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);

	const ImVec2 vContentRegionMin = ImGui::GetWindowContentRegionMin();

	mouse_x -= App->renderer->GetSceneWindowStartPos().x + vContentRegionMin.x;
	mouse_y -= App->renderer->GetSceneWindowStartPos().y + vContentRegionMin.y;

	float width = App->renderer->GetSceneWindowSize().x;
	float height = App->renderer->GetSceneWindowSize().y;

	return mouse_x > 0 && mouse_x < width && mouse_y > 0 && mouse_y < height;
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
		ImGui::DragFloat("Z-far", &Config::m_ZFar, 5.0f, 6.0f, 99999.0f, "%.2f");
	
		ImGui::Text("Vectors");
		ImGui::DragFloat3("Cam pos", &Config::m_CamPosition[0], 1.0f, -25.0f, 25.0f, "%.2f");

		ImGui::Checkbox("Draw ray", &m_drawRay);
		if (m_drawRay)
		{
			App->debugDraw->DrawRay(rayFrom, rayTo);
		}
		
		ViewProjectionMatrix();
	}
}
