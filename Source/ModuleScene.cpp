#include "ModuleScene.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleResources.h"

#include "Model.h"

#include "SDL.h"
#include "CCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <string>

#define MAX_BOUNDARY vec(200, 200, 200)
#define MIN_BOUNDARY vec(-200, -200, -200)

ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	MY_LOG("Model: Model creation");
    
    m_Root = new GameObject("Root", nullptr);
    m_Root->m_GUID = 0;
    m_GOSelected = nullptr;

    qt = new Quadtree();
    AABB boundaries = AABB(MIN_BOUNDARY, MAX_BOUNDARY);
    qt->SetBoundaries(boundaries);

    //GameObject* camera = m_Root->AddChild("Camera");
    //camera->AddComponent(new CCamera(true, camera));

    //LoadModel(".\\Assets\\Models\\BakerHouse.fbx");

    rapidjson::Document d;
    if (App->resources->LoadFile(".\\Assets\\Scene.scene", d))
        LoadScene(d);
    
    currentGizmoOperation = ImGuizmo::TRANSLATE;

	return true;
}

bool ModuleScene::CleanUp()
{
    delete qt;

    delete m_Root;

	return true;
}

GameObject* ModuleScene::CreateGameObject(const char* _name)
{
    return m_Root->AddChild(_name);
}

void ModuleScene::DrawImGuiInspector()
{
    if (m_GOSelected != nullptr) 
        m_GOSelected->DrawImGui();
}

void ModuleScene::SelectGameObject(GameObject* go)
{
    if (m_GOSelected != nullptr && (go != m_GOSelected || go == nullptr))
        m_GOSelected->m_SelectedFlag = false;

    if (go == nullptr)
        m_GOSelected = nullptr;
    else 
    {
        go->m_SelectedFlag = !go->m_SelectedFlag;
        if (go->m_SelectedFlag)
            m_GOSelected = go;
        else
            m_GOSelected = nullptr;
    }
}

void ModuleScene::RecursiveHierarchy(GameObject* go, GameObject*& node_clicked)
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

    if (go->m_SelectedFlag)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    if (go->m_Children.size() <= 0)
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    if (!go->m_Active)
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, color);

    bool node_open = ImGui::TreeNodeEx(go, node_flags, go->m_Name.c_str());
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        node_clicked = go;
        m_GODrag = go;
    }

    CheckHoverHierarchy(go);
    /*if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
        ImGui::Text("This is a drag and drop source");
        ImGui::EndDragDropSource();
    }*/

    if (node_open && go->m_Children.size() > 0)
    {
        for (int i = 0; i < go->m_Children.size(); ++i)
        {
            RecursiveHierarchy(go->m_Children[i], node_clicked);
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void ModuleScene::CheckHoverHierarchy(GameObject* go)
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
    {
        if (m_GODrag && m_GODrag != go)
        {
            ImGui::BeginTooltip();
            ImGui::Text("Move %s to %s", m_GODrag->m_Name, go->m_Name);
            ImGui::EndTooltip();
        }

        if (m_GODrag && ImGui::IsMouseReleased(0) && m_GODrag != go)
        {
            m_GODragParent = go;
        }
    }
}

void ModuleScene::DrawImGuiHierarchy()
{
    // Create empty gameobject button
    if (ImGui::Button("Create empty gameobject"))
        CreateGameObject("Empty GameObject");

    // Draw hierarchy
    ImGui::SetNextItemOpen(ImGuiTreeNodeFlags_DefaultOpen);
    if (ImGui::TreeNode("Root"))
    {
        static int selection_mask = 0;

        GameObject* node_clicked = nullptr;
        for (unsigned int i = 0; i < m_Root->m_Children.size(); ++i)
            RecursiveHierarchy(m_Root->m_Children[i], node_clicked);

        if (m_GODragParent) 
        {
            // TODO: check if its legal
            m_GODrag->SetParent(m_GODragParent);
            m_GODrag = nullptr;
            m_GODragParent = nullptr;
        }

        if (node_clicked != nullptr)
        {
            SelectGameObject(node_clicked);
        }
        ImGui::TreePop();
    }
    if (m_GODrag && ImGui::IsMouseReleased(0))
        m_GODrag = nullptr;
}

void ModuleScene::DrawImGuiToolBar()
{
    if (ImGui::Button("Translate")) currentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::Button("Rotate")) currentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::Button("Scale")) currentGizmoOperation = ImGuizmo::SCALE;
}

void ModuleScene::Draw(unsigned int program)
{
    // Gizmos // TODO gizmos controller
    if (m_GOSelected != nullptr)
    {
        if (App->input->GetKey(SDL_SCANCODE_T) == KeyState::KEY_DOWN)
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (App->input->GetKey(SDL_SCANCODE_R) == KeyState::KEY_DOWN)
            currentGizmoOperation = ImGuizmo::ROTATE;
        if (App->input->GetKey(SDL_SCANCODE_Y) == KeyState::KEY_DOWN)
            currentGizmoOperation = ImGuizmo::SCALE;

        float4x4 mat4 = App->scene->GetSelectedGO()->m_Transform->m_AccumulativeModelMatrix.Transposed();
        float4x4 view = App->camera->GetView().Transposed();
        float4x4 proj = App->camera->GetProj().Transposed();

        const ImVec2 newViewportPosition = ImGui::GetWindowPos();
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        float2 viewportPanelSize = App->renderer->GetSceneWindowSize();
        ImGuizmo::AllowAxisFlip(false);
        ImGuizmo::SetRect(newViewportPosition.x + vMin.x, newViewportPosition.y + vMin.y, viewportPanelSize.x, viewportPanelSize.y);
        if (ImGuizmo::Manipulate(&view[0][0], &proj[0][0], currentGizmoOperation, ImGuizmo::LOCAL, &mat4[0][0], NULL, NULL))
            m_GOSelected->m_Transform->GizmoTransformChange(mat4.Transposed());
    }

    m_Root->Update();

    if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && m_GOSelected != nullptr)
    {
        m_GOSelected->m_DeleteFlag = true;
        m_GOSelected = nullptr;
    }

    if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && m_GOSelected != nullptr)
    {
        m_GOSelected->Clone(m_GOSelected->m_Parent);
    }
}

void ModuleScene::LoadModel(Model* _model)
{
    GameObject* exportedGO = _model->ExportToGO(m_Root);
    AddToQuadtreeIfHasMesh(qt, exportedGO);
    SelectGameObject(exportedGO);
}

void ModuleScene::LoadTempScene()
{
    LoadScene(m_TempScene);
}

void ModuleScene::LoadScene(const rapidjson::Document& d)
{
    m_GOSelected = nullptr;

    qt->Clear();

    delete m_Root;
    m_Root = new GameObject("Root", nullptr);
    m_Root->m_GUID = 0;

    for (rapidjson::Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
    {
        m_Root->AddChild((*itr)["Name"].GetString())->OnLoad(*itr);
    }

    AddToQuadtreeIfHasMesh(qt, m_Root);
}

void ModuleScene::SaveTempScene()
{
    SaveScene(m_TempScene);
}

void ModuleScene::SaveScene(rapidjson::Document& d)
{
    d.SetArray();
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    for (unsigned int i = 0; i < m_Root->m_Children.size(); ++i) 
    {
        rapidjson::Value go;
        go.SetObject();
        m_Root->m_Children[i]->OnSave(go, allocator);
        d.PushBack(go, allocator);
    }
}

void ModuleScene::AddToQuadtreeIfHasMesh(Quadtree* qt, GameObject* go)
{
    for (int i = 0; i < go->m_Components.size(); ++i)
    {
        if (go->m_Components[i]->m_Type == ComponentType::MESH) 
        {
            qt->InsertGO(go);
            break;
        }
    }

    for (int i = 0; i < go->m_Children.size(); ++i)
        AddToQuadtreeIfHasMesh(qt, go->m_Children[i]);
}

void ModuleScene::RecursiveSearch(GameObject* _go, bool ancestorsActive, bool remove, bool firstFrame) //TODO: check start/enable/disable behaviours
{
    _go->m_InFrustum = false;

    if (_go->m_DeleteFlag || remove)
    {
        for (unsigned int i = 0; i < _go->m_Children.size(); ++i) 
            RecursiveSearch(_go->m_Children[i], ancestorsActive, true, firstFrame);

        for (unsigned int i = 0; i < _go->m_Components.size(); ++i)
        {
            if (_go->m_Components[i]->m_Type == ComponentType::MESH)
            {
                qt->EraseGO(_go);
                break;
            }
        }

        if (_go->m_DeleteFlag)
        {
            _go->m_Parent->RemoveChild(_go);
            delete _go;
        }
        return;
    }

    // First frame
    if (firstFrame)
    {
        _go->m_ActiveFlag = _go->m_Active;
        if (ancestorsActive && _go->m_Active)
            _go->Start();

        for (unsigned int i = 0; i < _go->m_Components.size(); ++i)
        {
            _go->m_Components[i]->m_EnableFlag = _go->m_Components[i]->m_Enabled;
            if (ancestorsActive && _go->m_Active && _go->m_Components[i]->m_Enabled)
                _go->m_Components[i]->Enable();
        }
    }
    else {
        if (_go->m_ActiveFlag != _go->m_Active)
        {
            _go->m_Active = _go->m_ActiveFlag;
            if (ancestorsActive && _go->m_Active)
                _go->Start();
        }

        unsigned int i = 0;
        bool hasMesh = false;
        unsigned int numberMeshes = 0;
        while (i < _go->m_Components.size())
        {
            if (_go->m_Components[i]->m_Type == ComponentType::MESH) 
            {
                hasMesh = true;
                numberMeshes += 1;
            }
            if (_go->m_Components[i]->m_DeleteFlag) {
                if (_go->m_Components[i]->m_Type == ComponentType::MESH) numberMeshes -= 1;
                _go->m_Components[i]->Disable();
                delete _go->m_Components[i];

                for (unsigned int j = i; j < _go->m_Components.size() - 1; ++j)
                    _go->m_Components[j] = _go->m_Components[j + 1];
                _go->m_Components.resize(_go->m_Components.size() - 1);
            }
            else
            {
                if (_go->m_Components[i]->m_EnableFlag != _go->m_Components[i]->m_Enabled)
                {
                    _go->m_Components[i]->m_Enabled = _go->m_Components[i]->m_EnableFlag;
                    if (_go->m_Components[i]->m_Enabled) _go->m_Components[i]->Enable();
                    else _go->m_Components[i]->Disable();
                }
                ++i;
            }
        }
        if (hasMesh && numberMeshes <= 0)
        {
            qt->EraseGO(_go);
        }
    }
    
    if (ancestorsActive && _go->m_Active)
    {
        for (unsigned int i = 0;  i < _go->m_Components.size(); ++i) 
        {
            if (_go->m_Active && _go->m_Components[i]->m_Enabled && _go->m_Components[i]->m_Type == ComponentType::LIGHT)
                m_Lights.push_back(((CLight*)_go->m_Components[i])->GetLightStruct(_go->m_Transform->GetForward(true), _go->m_Transform->GetPos()));
        }
    }

    for (unsigned int i = 0; i < _go->m_Children.size(); ++i)
        RecursiveSearch(_go->m_Children[i], ancestorsActive && _go->m_Active, firstFrame);
}

void ModuleScene::DrawImGui()
{
    if (ImGui::CollapsingHeader("Scene"))
    {
        ImGui::Checkbox("Draw quadtree nodes", &m_drawQuadtree);
        if (m_drawQuadtree)
        {
            int numberOfNodes = 0;
            qt->DrawAABB(numberOfNodes);
            ImGui::Text("There are %i nodes.", numberOfNodes);
        }
    }
}
