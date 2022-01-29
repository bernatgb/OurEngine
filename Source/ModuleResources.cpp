#include "ModuleResources.h"

#include "Application.h"
#include "ModuleScene.h"

#include "SceneImporter.h"
#include "Model.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

ModuleResources::ModuleResources()
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
    MY_LOG("ModuleResources: Load Resources");

    importer::LoadResources(m_Meshes, m_Textures, m_Materials, m_Models);

    return true;
}

bool ModuleResources::CleanUp()
{
    importer::SaveResources(m_Meshes, m_Textures, m_Materials, m_Models);

    for (auto it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
        delete it->second;

    for (auto it = m_Textures.begin(); it != m_Textures.end(); ++it)
        delete it->second;

    for (auto it = m_Materials.begin(); it != m_Materials.end(); ++it)
        delete it->second;

    for (auto it = m_Models.begin(); it != m_Models.end(); ++it)
        delete it->second;

    return true;
}

void ModuleResources::DrawImGuiResources()
{
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
            rapidjson::Document d;
            if (importer::LoadFile(filePathName.c_str(), d))
            {
                App->scene->LoadScene(d);
                MY_LOG("Scene Loaded");
            }
            else
                MY_LOG("Error while loading scene: %s", filePathName.c_str());
        }

        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::SameLine();

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
            App->scene->SaveScene(d);
            importer::SaveFile(filePathName.c_str(), d);
            MY_LOG("Scene Saved");
        }

        ImGuiFileDialog::Instance()->Close();
    }

    // TODO: CREATE MATERIAL

    ImGui::Separator();

    for (auto it = m_Models.begin(); it != m_Models.end(); ++it)
    {
        if (ImGui::Button(it->first.c_str()))
            App->scene->LoadModel(it->second);
    }
}

void ModuleResources::LoadFile(const char* _fileName, rapidjson::Document& d)
{
    importer::LoadFile(_fileName, d);
}

void ModuleResources::LoadResource(const char* _fileName)
{
    std::string extension = _fileName;
    const size_t last_slash_idx = extension.rfind('.');
    extension = extension.substr(last_slash_idx + 1, extension.length());

    if (extension == "fbx" || extension == "FBX")
        LoadModel(_fileName);
    else if (extension == "png" || extension == "jpg" || extension == "tif" || extension == "PNG" || extension == "JPG" || extension == "TIF")
    {
        Texture* newTexture = new Texture();
        importer::texture::Import(_fileName, newTexture, nullptr);
        m_Textures[newTexture->m_GUID] = newTexture;
    }
}

void ModuleResources::LoadModel(const char* _fileName)
{
    std::string modelName = _fileName;
    const size_t last_slash_idx = modelName.rfind('\\');
    if (std::string::npos != last_slash_idx)
        modelName = modelName.substr(last_slash_idx + 1, modelName.length());

    std::map<std::string, Model*>::iterator it = m_Models.find(modelName.c_str());
    if (it != m_Models.end())
    {
        App->scene->LoadModel(it->second);
    }
    else 
    {
        Model* model = importer::LoadModel(_fileName);
        m_Models[model->m_Name] = model;

        App->scene->LoadModel(model);
    }
}

Model* ModuleResources::FindModel(std::string _modelName)
{
    std::map<std::string, Model*>::iterator it = m_Models.find(_modelName);
    if (it != m_Models.end())
        return it->second;
    return nullptr;
}

Mesh* ModuleResources::FindMesh(unsigned int _meshId)
{
    std::map<unsigned int, Mesh*>::iterator it = m_Meshes.find(_meshId);
    if (it != m_Meshes.end())
        return it->second;
    return nullptr;
}

Material* ModuleResources::FindMaterial(unsigned int _materialId)
{
    std::map<unsigned int, Material*>::iterator it = m_Materials.find(_materialId);
    if (it != m_Materials.end())
        return it->second;
    return nullptr;
}

Texture* ModuleResources::FindTexture(unsigned int _textureId)
{
    std::map<unsigned int, Texture*>::iterator it = m_Textures.find(_textureId);
    if (it != m_Textures.end())
        return it->second;
    return nullptr;
}
