#include "GameObject.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "GL/glew.h"

#include "SceneImporter.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

GameObject::GameObject(const char* _name, GameObject* _parent)
{
	m_GUID = rand();

	m_Name = _name;
	m_Active = true;
	m_ActiveFlag = true;
	m_SelectedFlag = false;
	m_DeleteFlag = false;
	m_InFrustum = false;
	m_Parent = _parent;

	m_Transform = new CTransform(true, this);

	m_aabb = AABB(vec(0,0,0), vec(0,0,0));
	m_showAABB = false;
}

GameObject::~GameObject()
{
	delete m_Transform;

	for (unsigned int i = 0; i < m_Components.size(); ++i)
		delete m_Components[i];
	
	for (unsigned int i = 0; i < m_Children.size(); ++i)
		delete m_Children[i];
}

void GameObject::Start()
{
	NotifyMovement(false);

	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Enabled)
			m_Components[i]->Enable();
	}
}

void GameObject::Update()
{
	if (!m_Active)
		return;

	if(m_InFrustum)
	{
		glUniformMatrix4fv(glGetUniformLocation(App->renderer->GetProgram(), "model"), 1, GL_TRUE, &m_Transform->m_AccumulativeModelMatrix[0][0]);
	}

	for (unsigned int i = 0; i < m_Components.size(); ++i)
		m_Components[i]->Update();

	for (unsigned int i = 0; i < m_Children.size(); ++i)
		m_Children[i]->Update();
}

void GameObject::OnSave(rapidjson::Value& node, rapidjson::Document::AllocatorType& allocator) const
{
	// Store GUID, Name, Active, Parend GUID
	node.AddMember("GUID", rapidjson::Value(m_GUID), allocator);
	node.AddMember("Name", rapidjson::Value(m_Name.c_str(), allocator), allocator);
	node.AddMember("Active", rapidjson::Value(m_Active), allocator);
	node.AddMember("Parent", rapidjson::Value(m_Parent->m_GUID), allocator);

	// Store components
	rapidjson::Value components(rapidjson::kArrayType);
	//  Transform
	rapidjson::Value newComponent(rapidjson::kObjectType);
	m_Transform->OnSave(newComponent, allocator);
	components.PushBack(newComponent, allocator);
	//  Other components
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		rapidjson::Value newComponent(rapidjson::kObjectType);
		m_Components[i]->OnSave(newComponent, allocator);
		components.PushBack(newComponent, allocator);
	}
	node.AddMember("Components", components, allocator);

	// Store children
	rapidjson::Value children(rapidjson::kArrayType);
	for (unsigned int i = 0; i < m_Children.size(); ++i)
	{
		//children.PushBack(m_Children[i]->m_GUID, allocator); //TODO: SHOULD ONLY STORE THE GUID
		rapidjson::Value newChild(rapidjson::kObjectType);
		m_Children[i]->OnSave(newChild, allocator);
		children.PushBack(newChild, allocator);
	}
	node.AddMember("Children", children, allocator);

	// Store Min & Max
	node.AddMember("Min", importer::Float3ToValue(m_Min, allocator), allocator);
	node.AddMember("Max", importer::Float3ToValue(m_Max, allocator), allocator);

	//bool m_Selected;
	//bool m_InFrustum;
}

void GameObject::OnLoad(const rapidjson::Value& node)
{
	// Load GUID, Name, Active, Parend GUID
	m_GUID = node["GUID"].GetInt();
	m_Name = node["Name"].GetString();
	m_Active = node["Active"].GetBool();
	unsigned int parentId = node["Parent"].GetInt();

	// Load Components
	rapidjson::Value::ConstValueIterator itr = node["Components"].Begin();
	//  Transform
	m_Transform->OnLoad(*itr);
	++itr;
	//  Other components
	for (itr; itr != node["Components"].End(); ++itr)
	{
		switch ((ComponentType)(*itr)["Type"].GetInt())
		{
		case ComponentType::MESH:
		{
			CMesh* newCMesh = new CMesh(true, this);
			newCMesh->OnLoad(*itr);
			AddComponent(newCMesh);

			break;
		}
		case ComponentType::CAMERA:
		{
			CCamera* newCCamera = new CCamera(true, this);
			newCCamera->OnLoad(*itr);
			AddComponent(newCCamera);
			break;
		}
		case ComponentType::LIGHT:
		{
			CLight* newLight = new CLight(true, this);
			newLight->OnLoad(*itr);
			AddComponent(newLight);
			break;
		}
		}
	}

	// Load children
	for (rapidjson::Value::ConstValueIterator itr = node["Children"].Begin(); itr != node["Children"].End(); ++itr)
	{
		AddChild((*itr)["Name"].GetString())->OnLoad(*itr);
	}

	// Load Min & Max
	m_Min = importer::ValueToFloat3(node["Min"]);
	m_Max = importer::ValueToFloat3(node["Max"]);

	RecalculateBB();
}

GameObject* GameObject::Clone(GameObject* _parent)
{
	GameObject* newGO = _parent->AddChild(m_Name.c_str());

	newGO->m_Active = m_Active;
	newGO->m_Transform->Copy(m_Transform);
	for (unsigned int i = 0; i < m_Components.size(); ++i)
		newGO->AddComponent(m_Components[i]->GetAClone(newGO));

	for (unsigned int i = 0; i < m_Children.size(); ++i)
		m_Children[i]->Clone(newGO);

	newGO->m_aabb = m_aabb;

	newGO->m_Min = m_Min;
	newGO->m_Max = m_Max;

	Quadtree* qt = App->scene->GetQuadtree();
	App->scene->AddToQuadtreeIfHasMesh(qt, newGO);

	return newGO;
}

bool GameObject::IsInFrustum()
{
	bool inFrustum = true;

	int j = m_Components.size();

	for (int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Type == ComponentType::MESH)
		{
			CMesh* cMesh = (CMesh*)m_Components[i];
			inFrustum = App->camera->BoxInFrustum(*App->camera->GetCullingCamera(), cMesh->m_BB);
		}
	}

	return inFrustum;
}

bool GameObject::IsInFrustumViaQuadtree(QuadtreeNode* qtn)
{
	vec* aabbCornerPoints = new vec[8];
	qtn->GetAABB().GetCornerPoints(aabbCornerPoints);

	if (App->camera->BoxInFrustum(*App->camera->GetCullingCamera(), aabbCornerPoints))
	{
		std::list<GameObject*> gameObjectsInThisNode = qtn->GetGameObjectsInThisNode();
		std::list<GameObject*>::iterator it = std::find(gameObjectsInThisNode.begin(), gameObjectsInThisNode.end(), this);
		if (it != gameObjectsInThisNode.end())
		{
			return true;
		}
		else
		{
			QuadtreeNode** nodesChildren = qtn->GetChildren();
			if (!qtn->IsLeaf())
			{
				for (int i = 0; i < 4; ++i)
				{
					if (IsInFrustumViaQuadtree(nodesChildren[i]))
						return true;
				}
			}
		}
	}
	delete aabbCornerPoints;

	return false;
}

void GameObject::NotifyMovement(bool notifiedByTransform)
{
	// Notify transform if the notification doesnt come from him
	if (!notifiedByTransform)
		m_Transform->NotifyMovement();

	bool hasMesh = false;

	// Notify other components
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		m_Components[i]->NotifyMovement();
		if (m_Components[i]->m_Type == ComponentType::MESH)
			hasMesh = true;
	}

	// If it has a mesh update the quadtree
	if (hasMesh)
	{
		RecalculateBB();

		Quadtree* qt = App->scene->GetQuadtree();
		qt->EraseGO(this);
		qt->InsertGO(this);
	}

	// Notify owner children
	for (unsigned int i = 0; i < m_Children.size(); ++i)
		m_Children[i]->NotifyMovement(false);
}

void GameObject::RecalculateBB()
{
	m_Min = float3::zero;
	m_Max = float3::zero;

	bool firstMesh = true;
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Type == ComponentType::MESH)
		{
			CMesh* cMesh = (CMesh*)m_Components[i];
			if (firstMesh)
			{
				m_Min = cMesh->GetMinPoint().Min(cMesh->GetMaxPoint());
				m_Max = cMesh->GetMaxPoint().Max(cMesh->GetMinPoint());
				firstMesh = false;
			}
			else
			{
				m_Min = m_Min.Min(cMesh->GetMinPoint());
				m_Min = m_Min.Min(cMesh->GetMaxPoint());
				m_Max = m_Max.Max(cMesh->GetMinPoint());
				m_Max = m_Max.Max(cMesh->GetMaxPoint());
			}
		}
	}
	m_aabb = AABB(m_Min, m_Max);
}

void GameObject::SetParent(GameObject* _go)
{
	if (m_Parent != nullptr)
		m_Parent->RemoveChild(this);
	_go->AddChild(this);
}

GameObject* GameObject::AddChild(const char* _name)
{
	GameObject* child = new GameObject(_name, this);
	m_Children.push_back(child);
	child->NotifyMovement(false);

	return child;
}

void GameObject::AddChild(GameObject* _go)
{
	_go->m_Parent = this;
	m_Children.push_back(_go);
	_go->NotifyMovement(false);
}

void GameObject::RemoveChild(GameObject* _go)
{
	bool found = false;
	for (unsigned int i = 0; i < m_Children.size(); ++i) 
	{
		if (found)
			m_Children[i-1] = m_Children[i];
		else if (m_Children[i] == _go)
			found = true;
	}

	if (found) 
		m_Children.pop_back();

	NotifyMovement(false);
}

void GameObject::DrawImGui()
{
	ImGui::InputText("Name", &m_Name);
	ImGui::Checkbox("Active", &m_ActiveFlag);
	if (m_InFrustum) ImGui::Text("Visible");
	else ImGui::Text("Not visible");
	//header

	ImGui::Text("Min: %f %f %f", m_Min.x, m_Min.y, m_Min.z);
	ImGui::Text("Max: %f %f %f", m_Max.x, m_Max.y, m_Max.z);

	m_Transform->DrawImGui(0);

	for (unsigned int i = 0; i < m_Components.size(); ++i) 
		m_Components[i]->DrawImGui(i);

	// Add component button
	ImGui::Spacing();
	if (ImGui::Button("Add component..."))
		ImGui::OpenPopup("add_component");
	ImGui::SameLine();
	if (ImGui::BeginPopup("add_component"))
	{
		ImGui::Text("Add Component");
		ImGui::Separator();
		if (ImGui::Selectable("Mesh"))
		{
			CMesh* newCMesh = new CMesh(true, this);
			AddComponent(newCMesh);
		}
		if (ImGui::Selectable("Camera"))
		{
			bool noCamera = true;
			for (unsigned int i = 0; i < m_Components.size(); ++i) 
			{
				if (m_Components[i]->m_Type == ComponentType::CAMERA)
				{
					noCamera = false;
					break;
				}
			}
			if (noCamera) 
			{
				CCamera* newCCamera = new CCamera(true, this);
				AddComponent(newCCamera);
			}
		}
		if (ImGui::Selectable("Light"))
		{
			CLight* newCLight = new CLight(true, this);
			AddComponent(newCLight);
		}
		ImGui::EndPopup();
	}
}

template<typename T>
T* GameObject::GetComponent()
{
	ComponentType type;
	switch (T)
	{
	case CTransform:
		type = ComponentType::TRANSFORM;
		return m_Transform;
		break;
	case CMesh:
		type = ComponentType::MESH;
		break;
	case CCamera:
		type = ComponentType::CAMERA;
		break;
	case CLight:
		type = ComponentType::LIGHT;
		break;
	default:
		return nullptr;
	}

	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Type == type)
			return (T)m_Components[i];
	}

	return nullptr;
}

void GameObject::AddComponent(Component* _newComponent)
{
	m_Components.push_back(_newComponent);
	if (_newComponent->m_Type == ComponentType::MESH)
		RecalculateBB();
}
