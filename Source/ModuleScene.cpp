#include "ModuleScene.h"

#include "Application.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	MY_LOG("Model: Model creation");

	modelObj = new Model(".\\assets\\Models\\BakerHouse.fbx");

	m_Root = new GameObject("Root", nullptr);

	return true;
}

bool ModuleScene::CleanUp()
{
	delete modelObj;

	return true;
}

void ModuleScene::DrawImGuiModel()
{
	modelObj->DrawImGui();
}

void ModuleScene::DrawImGuiHierarchy()
{
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
    }
}

void ModuleScene::Draw(unsigned int program)
{
	modelObj->Draw(program);
}

void ModuleScene::LoadModel(const char* _fileName)
{
    delete modelObj;
    modelObj = new Model(_fileName);

    App->camera->AdjustToModel(modelObj);
}