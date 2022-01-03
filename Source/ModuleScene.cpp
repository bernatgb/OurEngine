#include "ModuleScene.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"

#include "Model.h"

#include "SDL.h"
#include "CCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "SceneImporter.h"

#include "ImGuiFileDialog.h"

#include <string>

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
    m_GOSelected = nullptr;

    importer::LoadResources(m_Meshes, m_Textures, m_Models);

    GameObject* camera = m_Root->AddChild("Camera");
    camera->AddComponent(new CCamera(true, camera));

    LoadModel(".\\Assets\\Models\\BakerHouse.fbx");

    //models.push_back(new Model(".\\assets\\Models\\BakerHouse.fbx"));
    //activeModel = 0;   

	return true;
}

bool ModuleScene::CleanUp()
{
    importer::SaveResources(m_Meshes, m_Textures, m_Models);

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

    ImGui::Separator();

    ImGui::Text("Things to ask to the teacher");
    ImGui::BulletText("Exporting model gameObject structre - Do it from assimp nodes");
    ImGui::BulletText("Mesh BB - destroy it");
    ImGui::BulletText("Shader in materials - yep");
    ImGui::BulletText("ImGui inputs: only in scene? - hover / improve input");
    ImGui::BulletText("Time and skybox images in ImGui");
    ImGui::BulletText("Binaries GUI (rand)");
    ImGui::BulletText("Binaries for texture");
    ImGui::BulletText("Json library");
    ImGui::BulletText("Where to do the FindMeshes & co");

    ImGui::Separator();

    ImGui::Text("TODO");
    ImGui::BulletText("Fix the f to work with gameobjects");
    ImGui::BulletText("GUIDs");
    ImGui::BulletText("Fix camera movement speed -> effected by fps");
    ImGui::BulletText("Fix mouse clicking");
    ImGui::BulletText("Have two cameras");
    ImGui::BulletText("Use correctly start and enable of components");

    ImGui::Separator();

    ImGui::Text("Corrections");
    ImGui::BulletText("overrides for the virtuals");
    ImGui::BulletText("const for const things");
    ImGui::BulletText("Make things private (sadly)");
    ImGui::BulletText("Try not to nest ifs");
    ImGui::BulletText("const & parameters");
    ImGui::BulletText("function names start with verbs");
    ImGui::BulletText("initialize in .h");

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

    for (auto it = m_Models.begin(); it != m_Models.end(); ++it)
    {
        if (ImGui::Button(it->first.c_str())) 
            SelectGameObject(it->second->ExportToGO(m_Root));
    }
}

void ModuleScene::Draw(unsigned int program)
{
    m_Root->Update();

    if (App->input->GetKey(SDL_SCANCODE_DELETE) && m_GOSelected != nullptr)
    {
        m_GOSelected->m_Parent->DeleteChild(m_GOSelected);
        m_GOSelected = nullptr;
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
        SelectGameObject(it->second->ExportToGO(m_Root));
        //App->camera->AdjustToModel(it->second);
        return;
    }
    
    Model* model = importer::LoadModel(_fileName);
    m_Models[model->m_Name] = model;

    SelectGameObject(m_Models[model->m_Name]->ExportToGO(m_Root));
    //App->camera->AdjustToModel(m_Models[model->m_Name]);
}

const GameObject* ModuleScene::GetRoot() const
{
    return m_Root;
}

void ModuleScene::LoadLibraryAssets()
{
}

void ModuleScene::SaveLibraryAssets()
{
}
