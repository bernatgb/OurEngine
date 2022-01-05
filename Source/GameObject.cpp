#include "GameObject.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "GL/glew.h"

#include "SceneImporter.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

GameObject::GameObject(const char* _name, GameObject* _parent)
{
	m_GUID = rand();

	m_Name = new char[100];
	strcpy(m_Name, _name);

	m_Active = true;
	m_Selected = false;
	m_InFrustum = true;
	m_Parent = _parent;

	m_Transform = new CTransform(true, this);
	m_Material = nullptr;

	m_aabb = AABB(vec(0,0,0), vec(0,0,0));
}

GameObject::~GameObject()
{
	delete[] m_Name;

	delete m_Transform;
	delete m_Material;

	for (unsigned int i = 0; i < m_Components.size(); ++i)
		delete m_Components[i];
	
	for (unsigned int i = 0; i < m_Children.size(); ++i)
		delete m_Children[i];
}

void GameObject::Start()
{
	if (m_Parent != nullptr) 
		m_Transform->NotifyMovement();

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

	if (m_Material != nullptr) 
	{
		m_Material->ActivateMaterial();
	}

	// Better in another place? Start / addComponent / notify movement
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Type == ComponentType::MESH)
		{
			CMesh* cMesh = (CMesh*)m_Components[i];

			m_Min = cMesh->m_MinPoint;
			m_Max = cMesh->m_MaxPoint;

			m_aabb = AABB(cMesh->m_MinPoint, cMesh->m_MaxPoint);
			ImGui::Text("%s aabb = (%f, %f, %f), (%f, %f, %f)", m_Name, cMesh->m_MinPoint.x, cMesh->m_MinPoint.y, cMesh->m_MinPoint.z,
				cMesh->m_MaxPoint.x, cMesh->m_MaxPoint.y, cMesh->m_MaxPoint.z);
		}
	}

	// Hierarchical frustum culling
	bool parentInFrustum = true;
	if (m_Parent != NULL) 
		parentInFrustum = m_Parent->m_InFrustum;

	/*
	std::vector<bool> childInFrustum;
	for (unsigned int i = 0; i < m_Children.size(); ++i)
	{
		childInFrustum.push_back(m_Children[i]->IsInFrustum());
	}

	bool allChildrenInFrustum = true;
	for (int i = 0; i < childInFrustum.size(); ++i)
		if (childInFrustum[i] == false)
			allChildrenInFrustum = false;

	m_InFrustum = allChildrenInFrustum;
	*/

	if (parentInFrustum && IsInFrustum())
	{
		glUniformMatrix4fv(glGetUniformLocation(App->renderer->program, "model"), 1, GL_TRUE, &m_Transform->m_AccumulativeModelMatrix[0][0]);
	}
	else
	{
		return;
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
	node.AddMember("Name", rapidjson::Value(m_Name, allocator), allocator);
	node.AddMember("Active", rapidjson::Value(m_Active), allocator);
	node.AddMember("Parent", rapidjson::Value(m_Parent->m_GUID), allocator);

	// Store components
	rapidjson::Value components(rapidjson::kArrayType);
	//  Transform
	rapidjson::Value newComponent(rapidjson::kObjectType);
	m_Transform->OnSave(newComponent, allocator);
	components.PushBack(newComponent, allocator);
	//  Material // TODO: should it be with the other components??
	if (m_Material != nullptr)
	{
		rapidjson::Value newComponent(rapidjson::kObjectType);
		m_Material->OnSave(newComponent, allocator);
		components.PushBack(newComponent, allocator);
	}
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
	strcpy(m_Name, node["Name"].GetString());
	m_Active = node["Active"].GetBool();
	unsigned int parentId = node["Parent"].GetInt();

	// Load Components
	rapidjson::Value::ConstValueIterator itr = node["Components"].Begin();
	//  Transform
	m_Transform->OnLoad(*itr);
	++itr;
	//  Material
	if (itr != node["Components"].End() && (*itr)["Type"].GetInt() == (int)ComponentType::MATERIAL)
	{
		m_Material = new CMaterial(true, this);
		m_Material->OnLoad(*itr);
		++itr;
	}
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
		case ComponentType::MATERIAL:
		{
			CMaterial* newCMaterial = new CMaterial(true, this);
			newCMaterial->OnLoad(*itr);
			AddComponent(newCMaterial);
			break;
		}
		case ComponentType::CAMERA:
		{
			CCamera* newCCamera = new CCamera(true, this);
			newCCamera->OnLoad(*itr);
			AddComponent(newCCamera);
			break;
		}
		}
	}
	// Load the transform component at the end
	//m_Transform->OnLoad(*node["Components"].Begin());

	// Load children
	for (rapidjson::Value::ConstValueIterator itr = node["Children"].Begin(); itr != node["Children"].End(); ++itr)
	{
		AddChild((*itr)["Name"].GetString())->OnLoad(*itr);
	}

	// Load Min & Max
	m_Min = importer::ValueToFloat3(node["Min"]);
	m_Max = importer::ValueToFloat3(node["Max"]);
}

bool GameObject::IsInFrustum()
{
	bool inFrustum = true;

	int j = m_Components.size();

	for (int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->m_Type == ComponentType::MESH)
		{
			ImGui::Text("%s", m_Name);
			CMesh* cMesh = (CMesh*)m_Components[i];
			inFrustum = App->camera->BoxInFrustum(*App->camera->GetGameCameraFrustum(), cMesh->m_BB);
			ImGui::SameLine();
			std::string sb = "no";
			if (inFrustum)
				sb = "yes";
			ImGui::Text("should be painted? %s", sb.c_str());
			// Better if we do this in another part?
		}
	}

	return inFrustum;
}

void GameObject::SetMaterial(Texture* _texture)
{
	m_Material = new CMaterial(true, this, _texture);
}

GameObject* GameObject::AddChild(const char* _name)
{
	GameObject* child = new GameObject(_name, this);
	m_Children.push_back(child);

	return child;
}

void GameObject::DeleteChild(GameObject* _go)
{
	bool found = false;
	for (unsigned int i = 0; i < m_Children.size(); ++i) 
	{
		if (found)
		{
			m_Children[i-1] = m_Children[i];
		}
		else if (m_Children[i] == _go)
		{
			delete m_Children[i];
			found = true;
		}
	}

	if (found) 
	{
		m_Children.pop_back();
	}
}

void GameObject::DrawImGui()
{
	ImGui::InputText("Name", m_Name, 100);
	ImGui::Checkbox("Active", &m_Active);
	//header

	m_Transform->DrawImGui();

	for (unsigned int i = 0; i < m_Components.size(); ++i) 
		m_Components[i]->DrawImGui();

	if (m_Material != nullptr)
		m_Material->DrawImGui();

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
		if (ImGui::Selectable("Material"))
		{
			CMaterial* newCMaterial = new CMaterial(true, this);
			AddComponent(newCMaterial);
		}
		if (ImGui::Selectable("Camera"))
		{
			CCamera* newCCamera = new CCamera(true, this);
			AddComponent(newCCamera);
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
		break;
	case CMesh:
		type = ComponentType::MESH;
		break;
	case CMaterial:
		type = ComponentType::MATERIAL;
		break;
	case CCamera:
		type = ComponentType::CAMERA;
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
}

/*template<typename T>
void GameObject::AddComponent(T* _newComponent)
{
	m_Components.push_back(_newComponent);
}*/

/*template CTransform* GameObject::GetComponent<CTransform>();
template CMaterial* GameObject::GetComponent<CMaterial>();
template CMesh* GameObject::GetComponent<CMesh>();

template void GameObject::AddComponent<CMesh>(CMesh*);*/