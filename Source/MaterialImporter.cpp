#include "MaterialImporter.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleScene.h"

#include "assimp/ai_assert.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "GL/glew.h"

#include <string>

void importer::texture::Import(const char* filePath, Texture* ourTexture, const char* fullPath)
{
	ourTexture->m_GUID = rand();
	ourTexture->m_Name = filePath;
	if (fullPath == nullptr) // filePath = fullpath 
	{
		const size_t last_slash_idx = ourTexture->m_Name.rfind('\\');
		if (std::string::npos != last_slash_idx)
			ourTexture->m_Name = ourTexture->m_Name.substr(last_slash_idx + 1, ourTexture->m_Name.length());
	}

	ourTexture->m_MinFilter = GL_LINEAR;
	ourTexture->m_MagFilter = GL_LINEAR;
	ourTexture->m_Wrap = GL_CLAMP;

	glGenTextures(1, &ourTexture->m_Texture);
	glBindTexture(GL_TEXTURE_2D, ourTexture->m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ourTexture->m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ourTexture->m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ourTexture->m_Wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ourTexture->m_Wrap);

	ourTexture->m_TextureData = nullptr;

	MY_LOG("Assimp texture (%s): Loading model from the path described in the FBX", filePath);
	ourTexture->m_TextureData = App->texture->LoadAndReturnTextureData(filePath);

	if (fullPath == nullptr && ourTexture->m_TextureData == nullptr) // filePath = fullpath 
	{
		MY_LOG("Assimp texture: Texture couldn't be loaded");
		return;
	}

	if (ourTexture->m_TextureData == nullptr)
	{
		std::string directoryPath = fullPath;
		const size_t last_slash_idx = directoryPath.rfind('\\');
		if (std::string::npos != last_slash_idx)
			directoryPath = directoryPath.substr(0, last_slash_idx) + '\\' + filePath;

		MY_LOG("Assimp texture (%s): Loading model from the same folder you of the FBX", directoryPath.c_str());
		ourTexture->m_TextureData = App->texture->LoadAndReturnTextureData(directoryPath.c_str());
		if (ourTexture->m_TextureData == nullptr)
		{
			directoryPath = TEXTURES_FOLDER;
			directoryPath += filePath;

			MY_LOG("Assimp texture (%s): Loading model from the Textures/ folder", directoryPath.c_str());
			ourTexture->m_TextureData = App->texture->LoadAndReturnTextureData(directoryPath.c_str());
			if (ourTexture->m_TextureData == nullptr)
			{
				MY_LOG("Assimp texture: Texture couldn't be loaded");
				return;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, ourTexture->m_TextureData->format, ourTexture->m_TextureData->width, ourTexture->m_TextureData->height, 0, ourTexture->m_TextureData->format, GL_UNSIGNED_BYTE, ourTexture->m_TextureData->data);

	MY_LOG("Assimp texture: Texture loaded correctly");
}

int importer::texture::Save(const Texture* ourTexture, char*& fileBuffer)
{
	unsigned int header[9] = {
		ourTexture->m_GUID,
		ourTexture->m_MinFilter,
		ourTexture->m_MagFilter,
		ourTexture->m_Wrap,
		ourTexture->m_TextureData->width,
		ourTexture->m_TextureData->height,
		ourTexture->m_TextureData->depth,
		ourTexture->m_TextureData->format,
		ourTexture->m_TextureData->bpp
	};

	unsigned int size = ourTexture->m_Name.length() + 1 + sizeof(header) + ourTexture->m_TextureData->bpp * ourTexture->m_TextureData->width * ourTexture->m_TextureData->height;
	
	// Allocate
	fileBuffer = new char[size];
	char* cursor = fileBuffer;

	// Store name
	unsigned int bytes = ourTexture->m_Name.length() + 1;
	cursor[0] = ourTexture->m_Name.length();
	for (unsigned int i = 0; i < ourTexture->m_Name.length(); ++i)
		cursor[i+1] = ourTexture->m_Name[i];
	cursor += bytes;

	// Store header
	bytes = sizeof(header);
	memcpy(cursor, header, sizeof(header));
	cursor += bytes;

	// Store texture
	bytes = ourTexture->m_TextureData->bpp * ourTexture->m_TextureData->width * ourTexture->m_TextureData->height;
	memcpy(cursor, ourTexture->m_TextureData->data, bytes);
	cursor += bytes;

	return size;
}

void importer::texture::Load(const char* fileBuffer, Texture* ourTexture)
{
	const char* cursor = fileBuffer;

	MY_LOG("MaterialImporter_Load: Reading main variables");
	ourTexture->m_Name = "";
	unsigned int bytes = cursor[0];
	for (unsigned int i = 0; i < bytes; ++i)
		ourTexture->m_Name += cursor[i + 1];
	cursor += (bytes + 1);

	unsigned int header[9];
	bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourTexture->m_GUID = header[0];
	ourTexture->m_MinFilter = header[1];
	ourTexture->m_MagFilter = header[2];
	ourTexture->m_Wrap = header[3];
	ourTexture->m_TextureData = new TextureData(false, 0, header[4], header[5], header[6], header[7], header[8], nullptr);
	
	bytes = ourTexture->m_TextureData->bpp * ourTexture->m_TextureData->width * ourTexture->m_TextureData->height;
	ourTexture->m_TextureData->data = new byte[bytes];
	memcpy(ourTexture->m_TextureData->data, cursor, bytes);
	cursor += bytes;

	//m_Name = new char[strlen(_fileName) + 1];
	//strcpy(m_Name, _fileName);

	//MY_LOG("Assimp texture (%s): Loading the texture file and setting its configuration", _fileName);

	// CREATING THE VBO
	MY_LOG("MaterialImporter_Load: Reading texture buffer");
	glGenTextures(1, &ourTexture->m_Texture);
	glBindTexture(GL_TEXTURE_2D, ourTexture->m_Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ourTexture->m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ourTexture->m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ourTexture->m_Wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ourTexture->m_Wrap);

	glTexImage2D(GL_TEXTURE_2D, 0, ourTexture->m_TextureData->format, ourTexture->m_TextureData->width, ourTexture->m_TextureData->height, 0, ourTexture->m_TextureData->format, GL_UNSIGNED_BYTE, ourTexture->m_TextureData->data);

	//glGenerateTextureMipmap(ourMaterial->m_Texture);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

// MATERIAL

void importer::material::Import(const aiMaterial* material, Material* ourMaterial, const char* fullPath)
{
	ourMaterial->m_GUID = rand();
	//ourTexture->m_Name;

	ourMaterial->m_DiffuseColor = float3(0.0f, 0.0f, 0.0f);
	ourMaterial->m_SpecularColor = float3(0.0f, 0.0f, 0.0f);
	ourMaterial->m_ShininessAlpha = false;
	ourMaterial->m_Shininess = 0.0f;

	aiString file;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) 
	{
		ourMaterial->m_DiffuseTexture = new Texture();
		importer::texture::Import(file.data, ourMaterial->m_DiffuseTexture, fullPath);
		App->scene->m_Textures[ourMaterial->m_DiffuseTexture->m_GUID] = ourMaterial->m_DiffuseTexture;
	}
	else
		MY_LOG("MaterialImporter: No diffuse texture");

	if (material->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS)
	{
		ourMaterial->m_SpecularTexture = new Texture();
		importer::texture::Import(file.data, ourMaterial->m_SpecularTexture, fullPath);
		App->scene->m_Textures[ourMaterial->m_SpecularTexture->m_GUID] = ourMaterial->m_SpecularTexture;
	}
	else
		MY_LOG("MaterialImporter: No specular texture");
}

int importer::material::Save(const Material* ourMaterial, char*& fileBuffer)
{
	unsigned int header[4] = {
		ourMaterial->m_GUID,
		(ourMaterial->m_DiffuseTexture != nullptr ? ourMaterial->m_DiffuseTexture->m_GUID : 0),
		(ourMaterial->m_SpecularTexture != nullptr ? ourMaterial->m_SpecularTexture->m_GUID : 0),
		(ourMaterial->m_ShininessAlpha ? 1 : 0),
	};

	float values[7] = {
		ourMaterial->m_DiffuseColor.x, ourMaterial->m_DiffuseColor.y, ourMaterial->m_DiffuseColor.z,
		ourMaterial->m_SpecularColor.x, ourMaterial->m_SpecularColor.y, ourMaterial->m_SpecularColor.z,
		ourMaterial->m_Shininess
	};

	unsigned int size = ourMaterial->m_Name.length() + 1 + sizeof(header) + sizeof(values);

	// Allocate
	fileBuffer = new char[size];
	char* cursor = fileBuffer;

	// Store name
	unsigned int bytes = ourMaterial->m_Name.length() + 1;
	cursor[0] = ourMaterial->m_Name.length();
	for (unsigned int i = 0; i < ourMaterial->m_Name.length(); ++i)
		cursor[i + 1] = ourMaterial->m_Name[i];
	cursor += bytes;

	// Store header
	bytes = sizeof(header);
	memcpy(cursor, header, sizeof(header));
	cursor += bytes;

	// Store values
	bytes = sizeof(values);
	memcpy(cursor, values, sizeof(values));
	cursor += bytes;

	return size;
}

void importer::material::Load(const char* fileBuffer, Material* ourMaterial)
{
	const char* cursor = fileBuffer;

	MY_LOG("MaterialImporter_Load: Reading main variables");
	unsigned int bytes = cursor[0];
	ourMaterial->m_Name = "";
	for (unsigned int i = 0; i < bytes; ++i)
		ourMaterial->m_Name += cursor[i + 1];
	strcpy(ourMaterial->m_AuxName, (ourMaterial->m_Name + "\n").c_str());
	cursor += (bytes + 1);


	unsigned int header[4];
	bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourMaterial->m_GUID = header[0];
	unsigned int m_DiffuseGUID = header[1];
	unsigned int m_SpecularGUID = header[2];
	ourMaterial->m_ShininessAlpha = header[3];

	if (m_DiffuseGUID != 0)
		ourMaterial->m_DiffuseTexture = App->scene->m_Textures[m_DiffuseGUID];
	if (m_SpecularGUID != 0)
		ourMaterial->m_SpecularTexture = App->scene->m_Textures[m_SpecularGUID];

	float values[7];
	bytes = sizeof(values);
	memcpy(values, cursor, bytes);
	cursor += bytes;

	ourMaterial->m_DiffuseColor = float3(values[0], values[1], values[2]);
	ourMaterial->m_SpecularColor = float3(values[3], values[4], values[5]);
	ourMaterial->m_Shininess = values[6];
}
