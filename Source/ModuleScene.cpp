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
    
    ot = new Octotree();
    boundaries = AABB(MIN_BOUNDARY, MAX_BOUNDARY);
    ot->SetBoundaries(boundaries);

    //GameObject* camera = m_Root->AddChild("Camera");
    //camera->AddComponent(new CCamera(true, camera));

    //LoadModel(".\\Assets\\Models\\BakerHouse.fbx");

    rapidjson::Document d;
    App->resources->LoadFile(".\\Assets\\Scene.scene", d);
    LoadScene(d);
    
    currentGizmoOperation = ImGuizmo::TRANSLATE;

	return true;
}

bool ModuleScene::CleanUp()
{
    delete qt;
    delete ot;

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
        m_GOSelected->m_Selected = false;

    if (go == nullptr)
        m_GOSelected = nullptr;
    else 
    {
        go->m_Selected = !go->m_Selected;
        if (go->m_Selected)
            m_GOSelected = go;
        else
            m_GOSelected = nullptr;
    }
}

void ModuleScene::RecursiveHierarchy(GameObject* go, GameObject*& node_clicked)
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

    if (go->m_Selected)
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
    /*if (ImGui::IsMouseDoubleClicked(0))
    {
        App->camera->AdjustToGO(go);
    }*/

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

            /*
            // Update selection state
            if (ImGui::GetIO().KeyCtrl)
                selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
            else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                selection_mask = (1 << node_clicked);           // Click to single-select
            */
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
        m_GOSelected->m_Parent->RemoveChild(m_GOSelected);
        delete m_GOSelected;
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
    ot->Clear();

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
            ot->InsertGO(go);
            break;
        }
    }

    for (int i = 0; i < go->m_Children.size(); ++i)
        AddToQuadtreeIfHasMesh(qt, go->m_Children[i]);
}

void ModuleScene::RecursiveSearch(GameObject* _go, bool ancestors, bool firstFrame) //TODO: check start/enable/disable behaviours
{
    _go->m_InFrustum = false;

    // First frame
    if (firstFrame)
    {
        _go->m_ActiveFlag = _go->m_Active;
        if (ancestors && _go->m_Active) 
            _go->Start();

        for (unsigned int i = 0; i < _go->m_Components.size(); ++i)
        {
            _go->m_Components[i]->m_EnableFlag = _go->m_Components[i]->m_Enabled;
            if (ancestors && _go->m_Active && _go->m_Components[i]->m_Enabled)
                _go->m_Components[i]->Enable();
        }
    }
    else {
        if (_go->m_ActiveFlag != _go->m_Active)
        {
            _go->m_Active = _go->m_ActiveFlag;
            if (ancestors && _go->m_Active)
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
            ot->EraseGO(_go);
        }
    }
    
    if (ancestors && _go->m_Active)
    {
        for (unsigned int i = 0;  i < _go->m_Components.size(); ++i) 
        {
            if (_go->m_Active && _go->m_Components[i]->m_Enabled && _go->m_Components[i]->m_Type == ComponentType::LIGHT)
                m_Lights.push_back(((CLight*)_go->m_Components[i])->GetLightStruct(_go->m_Transform->GetForward(true), _go->m_Transform->GetPos()));
        }
    }

    for (unsigned int i = 0; i < _go->m_Children.size(); ++i)
        RecursiveSearch(_go->m_Children[i], ancestors && _go->m_Active, firstFrame);
}


/* IMGUI CODE FOR DRAG AND DROP / REORDER

if (ImGui::TreeNode("ImGui demo: Advanced, with Selectable nodes"))
{
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
    static bool test_drag_and_drop = false;

    ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);

    // 'selection_mask' is dumb representation of what may be user-side selection state.
    //  You may retain selection state inside or outside your objects in whatever format you see fit.
    // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
    /// of the loop. May be a pointer to your own node type, etc.
    static int selection_mask = (1 << 2);
    int node_clicked = -1;
    for (int i = 0; i < 6; i++)
    {
        // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
        // To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
        ImGuiTreeNodeFlags node_flags = base_flags;
        const bool is_selected = (selection_mask & (1 << i)) != 0;
        if (is_selected)
            node_flags |= ImGuiTreeNodeFlags_Selected;
        if (i < 3)
        {
            // Items 0..2 are Tree Node
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                node_clicked = i;
            if (test_drag_and_drop && ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                ImGui::Text("This is a drag and drop source");
                ImGui::EndDragDropSource();
            }
            if (node_open)
            {
                ImGui::BulletText("Blah blah Blah Blah");
                ImGui::TreePop();
            }
        }
        else
        {
            // Items 3..5 are Tree Leaves
            // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
            // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
            ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                node_clicked = i;
            if (test_drag_and_drop && ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                ImGui::Text("This is a drag and drop source");
                ImGui::EndDragDropSource();
            }
        }
    }
    if (node_clicked != -1)
    {
        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        if (ImGui::GetIO().KeyCtrl)
            selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
        else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
            selection_mask = (1 << node_clicked);           // Click to single-select
    }
    ImGui::TreePop();
}


if (ImGui::TreeNode("Drag to reorder items (simple)"))
{
    // Simple reordering
    static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
    for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
    {
        const char* item = item_names[n];
        ImGui::Selectable(item);

        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
        {
            int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
            if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
            {
                item_names[n] = item_names[n_next];
                item_names[n_next] = item;
                ImGui::ResetMouseDragDelta();
            }
        }
    }
    ImGui::TreePop();
}*/