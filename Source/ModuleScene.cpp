#include "ModuleScene.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleRender.h"

#include "SceneImporter.h"

#include "Model.h"

#include "SDL.h"
#include "CCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

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

    importer::LoadResources(m_Meshes, m_Textures, m_Models);

    GameObject* camera = m_Root->AddChild("Camera");
    camera->AddComponent(new CCamera(true, camera));

    qt = new Quadtree();
    AABB boundaries = AABB(MIN_BOUNDARY, MAX_BOUNDARY);
    qt->SetBoundaries(boundaries);

    LoadModel(".\\Assets\\Models\\BakerHouse.fbx");

    //models.push_back(new Model(".\\assets\\Models\\BakerHouse.fbx"));
    //activeModel = 0;   

    currentGizmoOperation = ImGuizmo::TRANSLATE;

	return true;
}

bool ModuleScene::CleanUp()
{
    importer::SaveResources(m_Meshes, m_Textures, m_Models);

    delete qt;

    delete m_Root;

    for (auto it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
        delete it->second;

    for (auto it = m_Textures.begin(); it != m_Textures.end(); ++it)
        delete it->second;

    for (auto it = m_Models.begin(); it != m_Models.end(); ++it)
        delete it->second;

    //for (unsigned int i = 0; i < models.size(); ++i)
    //    delete models[i];

	return true;
}

GameObject* ModuleScene::CreateGameObject(const char* _name)
{
    return m_Root->AddChild(_name);
}

void ModuleScene::DrawImGuiModel()
{
    if (m_GOSelected != nullptr) 
        m_GOSelected->DrawImGui();
}

void ModuleScene::SelectGameObject(GameObject* go)
{
    if (m_GOSelected != nullptr && go != m_GOSelected)
        m_GOSelected->m_Selected = false;

    go->m_Selected = !go->m_Selected;
    if (go->m_Selected)
        m_GOSelected = go;
    else
        m_GOSelected = nullptr;
}

void ModuleScene::RecursiveHierarchy(GameObject* go, GameObject*& node_clicked)
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

    if (go->m_Selected)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    if (go->m_Children.size() <= 0)
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    bool node_open = ImGui::TreeNodeEx(go, node_flags, go->m_Name);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        node_clicked = go;
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
        ImGui::Text("This is a drag and drop source");
        ImGui::EndDragDropSource();
    }

    if (node_open && go->m_Children.size() > 0)
    {
        for (int i = 0; i < go->m_Children.size(); ++i)
        {
            RecursiveHierarchy(go->m_Children[i], node_clicked);
        }
        ImGui::TreePop();
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

    //// NOTEBOOK PROPOUSE
    ImGui::Separator();

    ImGui::Text("Things to ask to the teacher");
    ImGui::BulletText("Mesh BB - destroy it");
    ImGui::BulletText("Shader in materials - yep");
    ImGui::BulletText("ImGui inputs: only in scene? - hover / improve input");
    ImGui::BulletText("Time and skybox images in ImGui");
    ImGui::BulletText("Where to do the FindMeshes & co");

    ImGui::Separator();

    ImGui::Text("TODO");
    ImGui::BulletText("Corrections to f (not calculating the distance properly) & orbit (errors in pols)");
    ImGui::BulletText("Fix camera movement speed -> effected by fps");
    ImGui::BulletText("Fix mouse clicking");
    ImGui::BulletText("Have two cameras");
    ImGui::BulletText("Use correctly start and enable of components");
    ImGui::BulletText("Currently not using pitch & yawn");

    ImGui::Separator();

    ImGui::Text("Corrections");
    ImGui::BulletText("overrides for the virtuals");
    ImGui::BulletText("const for const things");
    ImGui::BulletText("Make things private (sadly)");
    ImGui::BulletText("Try not to nest ifs");
    ImGui::BulletText("const & parameters");
    ImGui::BulletText("function names start with verbs");
    ImGui::BulletText("initialize in .h");

    /*
    MIQUEL:
    -SCENE SERIALIZATION
    -FILE SYSTEM


    -ASSIMP TRANSFORMS
    -ASPECTS (WINDOW/IMGUI)
    -GAME VIEW
    -FIX ENABLED / START
    -SERIALIZATION OF GAMEOBJECTS USING GUID
    -FIX GO MIN & MAX
    -WORKING HIERARCHY
    -IMPORVE INPUT MODULE TO WORK AS THE 2D EXERCISE


    BERNAT:
    -MOUSE/GIZMOS
    -PBR
    */
    ////

    /*if (ImGui::TreeNode("ImGui demo: Advanced, with Selectable nodes"))
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
}

void ModuleScene::DrawImGuiResources()
{
    //// TEST
    if (ImGui::Button("Open File Dialog"))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp", ".");

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
            MY_LOG("File selected");
            MY_LOG(filePathName.c_str());
            MY_LOG(filePath.c_str());
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
    ////

    // Load scene
    if (ImGui::Button("Load Scene"))
        ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Load Scene", ".scene", ".");

    if (ImGuiFileDialog::Instance()->Display("LoadScene"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            MY_LOG("Loading scene: %s", filePathName.c_str());
            m_GOSelected = nullptr;
            rapidjson::Document d;
            if (importer::LoadFile(filePathName.c_str(), d))
            {
                LoadScene(d);
                MY_LOG("Scene Loaded");
            }
            else
                MY_LOG("Error while loading scene: %s", filePathName.c_str());
        }

        ImGuiFileDialog::Instance()->Close();
    }

    // Save scene
    if (ImGui::Button("Save Scene"))
        ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "Save Scene", ".scene", ".");

    if (ImGuiFileDialog::Instance()->Display("SaveScene"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            MY_LOG("Saving scene at: %s", filePathName.c_str());
            rapidjson::Document d;
            SaveScene(d);
            importer::SaveFile(filePathName.c_str(), d);
            MY_LOG("Scene Saved");
        }

        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::Separator();

    for (auto it = m_Models.begin(); it != m_Models.end(); ++it)
    {
        if (ImGui::Button(it->first.c_str())) 
        {
            GameObject* exportedGO = it->second->ExportToGO(m_Root);
            AddToQuadtreeIfHasMesh(qt, exportedGO);
            SelectGameObject(exportedGO);
        }
    }
}

void ModuleScene::Draw(unsigned int program)
{
    // Gizmos
    if (m_GOSelected != nullptr)
    {
        if (App->input->GetKey(SDL_SCANCODE_T) == KeyState::KEY_DOWN)
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (App->input->GetKey(SDL_SCANCODE_R) == KeyState::KEY_DOWN)
            currentGizmoOperation = ImGuizmo::ROTATE;
        if (App->input->GetKey(SDL_SCANCODE_Y) == KeyState::KEY_DOWN)
            currentGizmoOperation = ImGuizmo::SCALE;

        float4x4 mat4 = App->scene->GetSelectedGO()->m_Transform->m_AccumulativeModelMatrix.Transposed();
        float4x4 view = App->camera->view.Transposed();
        float4x4 proj = App->camera->proj.Transposed();

        const ImVec2 newViewportPosition = ImGui::GetWindowPos();
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        float2 viewportPanelSize = App->renderer->GetSceneWindowSize();
        ImGuizmo::SetRect(newViewportPosition.x + vMin.x, newViewportPosition.y + vMin.y, viewportPanelSize.x, viewportPanelSize.y);
        if (ImGuizmo::Manipulate(&view[0][0], &proj[0][0], currentGizmoOperation, ImGuizmo::LOCAL, &mat4[0][0], NULL, NULL))
            m_GOSelected->m_Transform->GizmoTransformChange(mat4.Transposed());
    }

    m_Root->Update();

    if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && m_GOSelected != nullptr)
    {
        m_GOSelected->m_Parent->DeleteChild(m_GOSelected);
        m_GOSelected = nullptr;
    }

    if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && m_GOSelected != nullptr)
    {
        m_GOSelected->Clone(m_GOSelected->m_Parent);
    }
}

void ModuleScene::LoadModel(const char* _fileName)
{
    /*
    std::string modelName = _fileName;
    const size_t last_slash_idx = modelName.rfind('\\');
    if (std::string::npos != last_slash_idx)
        modelName = modelName.substr(last_slash_idx, modelName.length());

    std::map<std::string, Model*>::iterator it = m_Models.find(modelName);
    if (it == m_Models.end()) 
    {
        //import new model
        //and save it
    }

    SelectGameObject(m_Models[modelName]->ExportToGO(m_Root));
    App->camera->AdjustToModel(m_Models[modelName]);*/

    /*activeModel = -1;
    for (unsigned int i = 0; i < models.size(); ++i)
    {
        unsigned int j = 0;
        while (models[i]->m_Name[j] == _fileName[j]) {
            if (models[i]->m_Name[j] == '\0' && _fileName[j] == '\0') 
            {
                activeModel = i;
                break;
            }
            ++j;
        }

        if (activeModel != -1)
            break;
    }

    if (activeModel == -1) 
    {
        models.push_back(new Model(_fileName));
        activeModel = models.size()-1;
    }

    SelectGameObject(models[activeModel]->ExportToGO(m_Root));

    App->camera->AdjustToModel(models[activeModel]);*/

    std::string modelName = _fileName;
    const size_t last_slash_idx = modelName.rfind('\\');
    if (std::string::npos != last_slash_idx)
        modelName = modelName.substr(last_slash_idx + 1, modelName.length());

    std::map<std::string, Model*>::iterator it = m_Models.find(modelName.c_str());
    if (it != m_Models.end()) 
    {
        GameObject* exportedGO = it->second->ExportToGO(m_Root);
        AddToQuadtreeIfHasMesh(qt, exportedGO);
        SelectGameObject(exportedGO);
        //App->camera->AdjustToModel(it->second);
        
        return;
    }
    
    Model* model = importer::LoadModel(_fileName);
    m_Models[model->m_Name] = model;

    GameObject* exportedGO = m_Models[model->m_Name]->ExportToGO(m_Root);
    AddToQuadtreeIfHasMesh(qt, exportedGO);
    SelectGameObject(exportedGO);
    //App->camera->AdjustToModel(m_Models[model->m_Name]);
}

void ModuleScene::LoadTempScene()
{
    LoadScene(m_TempScene);
}

void ModuleScene::LoadScene(const rapidjson::Document& d)
{
    m_GOSelected = nullptr;

    delete m_Root;
    m_Root = new GameObject("Root", nullptr);
    m_Root->m_GUID = 0;

    for (rapidjson::Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
    {
        m_Root->AddChild((*itr)["Name"].GetString())->OnLoad(*itr);
    }
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
            qt->InsertGO(go);
    }

    for (int i = 0; i < go->m_Children.size(); ++i)
        AddToQuadtreeIfHasMesh(qt, go->m_Children[i]);
}

void ModuleScene::RecursiveSearch(GameObject* _go)
{
    //TODO: CHECK OBJECT FLAGS

    //TODO: CHECK COMPONENTS FLAGS

    for (unsigned int i = 0; i < _go->m_Components.size(); ++i)
    {
        if (_go->m_Components[i]->m_Type == ComponentType::LIGHT)
            m_Lights.push_back(((CLight*)_go->m_Components[i])->GetLightStruct(_go->m_Transform->GetForward(true), _go->m_Transform->GetPos()));
    }

    for (unsigned int i = 0; i < _go->m_Children.size(); ++i) 
    {
        RecursiveSearch(_go->m_Children[i]);
    }
}

/*
Light GetLightStruct(CLight* _light) {
    Light light;
    light.lightType = (int)_light->m_Type;
    light.lightColor = _light->m_Color;
    light.intensity = _light->m_Intensity;
    light.radius = _light->m_Radius;
    light.innerAngle = _light->m_InnerAngle * DEGTORAD;
    light.outerAngle = _light->m_OuterAngle * DEGTORAD;

    light.direction = _light->m_Owner->m_Transform->GetForward(true);
    light.position = _light->m_Owner->m_Transform->GetPos();

    return light;
};*/