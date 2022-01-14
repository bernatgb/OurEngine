#include "MaterialImporter.h"

#include "Application.h"
#include "ModuleTexture.h"

#include "assimp/ai_assert.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "GL/glew.h"

#include <string>

void importer::material::Import(const aiMaterial* material, Texture* ourMaterial, const char* fullPath)
{
	aiString file;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &file) != AI_SUCCESS)
		return;

	ourMaterial->m_GUID = rand();
	ourMaterial->m_Name = file.data;

	ourMaterial->m_MinFilter = GL_LINEAR;
	ourMaterial->m_MagFilter = GL_LINEAR;
	ourMaterial->m_Wrap = GL_CLAMP;

	glGenTextures(1, &ourMaterial->m_Texture);
	glBindTexture(GL_TEXTURE_2D, ourMaterial->m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ourMaterial->m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ourMaterial->m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ourMaterial->m_Wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ourMaterial->m_Wrap);

	ourMaterial->m_TextureData = nullptr;

	MY_LOG("Assimp texture (%s): Loading model from the path described in the FBX", file.data);
	ourMaterial->m_TextureData = App->texture->LoadAndReturnTextureData(file.data);
	if (ourMaterial->m_TextureData == nullptr)
	{
		std::string directoryPath = fullPath;
		const size_t last_slash_idx = directoryPath.rfind('\\');
		if (std::string::npos != last_slash_idx)
			directoryPath = directoryPath.substr(0, last_slash_idx) + '\\' + file.data;

		MY_LOG("Assimp texture (%s): Loading model from the same folder you of the FBX", directoryPath.c_str());
		ourMaterial->m_TextureData = App->texture->LoadAndReturnTextureData(directoryPath.c_str());
		if (ourMaterial->m_TextureData == nullptr)
		{
			directoryPath = TEXTURES_FOLDER;
			directoryPath += file.data;

			MY_LOG("Assimp texture (%s): Loading model from the Textures/ folder", directoryPath.c_str());
			ourMaterial->m_TextureData = App->texture->LoadAndReturnTextureData(directoryPath.c_str());
			if (ourMaterial->m_TextureData == nullptr)
			{
				MY_LOG("Assimp texture: Texture couldn't be loaded");
				return;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, ourMaterial->m_TextureData->format, ourMaterial->m_TextureData->width, ourMaterial->m_TextureData->height, 0, ourMaterial->m_TextureData->format, GL_UNSIGNED_BYTE, ourMaterial->m_TextureData->data);

	// SPECULAR
	if (material->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS) 
	{
		glGenTextures(1, &ourMaterial->m_SpecularTexture);
		glBindTexture(GL_TEXTURE_2D, ourMaterial->m_SpecularTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ourMaterial->m_MinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ourMaterial->m_MagFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ourMaterial->m_Wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ourMaterial->m_Wrap);

		ourMaterial->m_SpecularTextureData = nullptr;

		MY_LOG("Assimp texture (%s): Loading model from the path described in the FBX", file.data);
		ourMaterial->m_SpecularTextureData = App->texture->LoadAndReturnTextureData(file.data);
		if (ourMaterial->m_SpecularTextureData == nullptr)
		{
			std::string directoryPath = fullPath;
			const size_t last_slash_idx = directoryPath.rfind('\\');
			if (std::string::npos != last_slash_idx)
				directoryPath = directoryPath.substr(0, last_slash_idx) + '\\' + file.data;

			MY_LOG("Assimp texture (%s): Loading model from the same folder you of the FBX", directoryPath.c_str());
			ourMaterial->m_SpecularTextureData = App->texture->LoadAndReturnTextureData(directoryPath.c_str());
			if (ourMaterial->m_SpecularTextureData == nullptr)
			{
				directoryPath = TEXTURES_FOLDER;
				directoryPath += file.data;

				MY_LOG("Assimp texture (%s): Loading model from the Textures/ folder", directoryPath.c_str());
				ourMaterial->m_SpecularTextureData = App->texture->LoadAndReturnTextureData(directoryPath.c_str());
				if (ourMaterial->m_SpecularTextureData == nullptr)
				{
					MY_LOG("Assimp texture: Texture couldn't be loaded");
					return;
				}
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, ourMaterial->m_SpecularTextureData->format, ourMaterial->m_SpecularTextureData->width, ourMaterial->m_SpecularTextureData->height, 0, ourMaterial->m_SpecularTextureData->format, GL_UNSIGNED_BYTE, ourMaterial->m_SpecularTextureData->data);
	}
	else 
	{
		MY_LOG("Assimp texture: There is no specular texture");
	}

	MY_LOG("Assimp texture: Texture loaded correctly");
}

int importer::material::Save(const Texture* ourMaterial, char*& fileBuffer)
{
	unsigned int header[10] = {
		ourMaterial->m_GUID,
		ourMaterial->m_MinFilter,
		ourMaterial->m_MagFilter,
		ourMaterial->m_Wrap,
		ourMaterial->m_TextureData->width,
		ourMaterial->m_TextureData->height,
		ourMaterial->m_TextureData->depth,
		ourMaterial->m_TextureData->format,
		ourMaterial->m_TextureData->bpp,
		(ourMaterial->m_SpecularTextureData != nullptr ? 1 : 0)
	};

	unsigned int size = ourMaterial->m_Name.length() + 1 + sizeof(header) + ourMaterial->m_TextureData->bpp * ourMaterial->m_TextureData->width * ourMaterial->m_TextureData->height;
	
	if (ourMaterial->m_SpecularTextureData != nullptr) 
		size += sizeof(int) * 5 + ourMaterial->m_SpecularTextureData->bpp * ourMaterial->m_SpecularTextureData->width * ourMaterial->m_SpecularTextureData->height;

	// Allocate
	fileBuffer = new char[size];
	char* cursor = fileBuffer;

	// Store name
	unsigned int bytes = ourMaterial->m_Name.length() + 1;
	cursor[0] = ourMaterial->m_Name.length();
	for (unsigned int i = 0; i < ourMaterial->m_Name.length(); ++i)
		cursor[i+1] = ourMaterial->m_Name[i];
	cursor += bytes;

	// Store header
	bytes = sizeof(header);
	memcpy(cursor, header, sizeof(header));
	cursor += bytes;

	// Store vertices
	bytes = ourMaterial->m_TextureData->bpp * ourMaterial->m_TextureData->width * ourMaterial->m_TextureData->height;
	memcpy(cursor, ourMaterial->m_TextureData->data, bytes);
	cursor += bytes;

	if (ourMaterial->m_SpecularTextureData != nullptr)
	{
		unsigned int secondHeader[5] = {
			ourMaterial->m_SpecularTextureData->width,
			ourMaterial->m_SpecularTextureData->height,
			ourMaterial->m_SpecularTextureData->depth,
			ourMaterial->m_SpecularTextureData->format,
			ourMaterial->m_SpecularTextureData->bpp
		};

		bytes = sizeof(secondHeader);
		memcpy(cursor, secondHeader, sizeof(secondHeader));
		cursor += bytes;

		bytes = ourMaterial->m_SpecularTextureData->bpp * ourMaterial->m_SpecularTextureData->width * ourMaterial->m_SpecularTextureData->height;
		memcpy(cursor, ourMaterial->m_TextureData->data, bytes);
		cursor += bytes;
	}

	return size;
}

void importer::material::Load(const char* fileBuffer, Texture* ourMaterial)
{
	const char* cursor = fileBuffer;

	MY_LOG("MaterialImporter_Load: Reading main variables");
	ourMaterial->m_Name = "";
	unsigned int bytes = cursor[0];
	for (unsigned int i = 0; i < bytes; ++i)
		ourMaterial->m_Name += cursor[i + 1];
	cursor += (bytes + 1);

	unsigned int header[10];
	bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourMaterial->m_GUID = header[0];
	ourMaterial->m_MinFilter = header[1];
	ourMaterial->m_MagFilter = header[2];
	ourMaterial->m_Wrap = header[3];
	ourMaterial->m_TextureData = new TextureData(false, 0, header[4], header[5], header[6], header[7], header[8], nullptr);

	bool specularTexture = header[9];

	bytes = ourMaterial->m_TextureData->bpp * ourMaterial->m_TextureData->width * ourMaterial->m_TextureData->height;
	ourMaterial->m_TextureData->data = new byte[bytes];
	memcpy(ourMaterial->m_TextureData->data, cursor, bytes);
	cursor += bytes;

	//m_Name = new char[strlen(_fileName) + 1];
	//strcpy(m_Name, _fileName);

	//MY_LOG("Assimp texture (%s): Loading the texture file and setting its configuration", _fileName);

	// CREATING THE VBO
	MY_LOG("MaterialImporter_Load: Reading texture buffer");
	glGenTextures(1, &ourMaterial->m_Texture);
	glBindTexture(GL_TEXTURE_2D, ourMaterial->m_Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ourMaterial->m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ourMaterial->m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ourMaterial->m_Wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ourMaterial->m_Wrap);

	glTexImage2D(GL_TEXTURE_2D, 0, ourMaterial->m_TextureData->format, ourMaterial->m_TextureData->width, ourMaterial->m_TextureData->height, 0, ourMaterial->m_TextureData->format, GL_UNSIGNED_BYTE, ourMaterial->m_TextureData->data);

	//glGenerateTextureMipmap(ourMaterial->m_Texture);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	if (specularTexture) 
	{
		unsigned int secondHeader[5];
		bytes = sizeof(secondHeader);
		memcpy(secondHeader, cursor, bytes);
		cursor += bytes;

		ourMaterial->m_SpecularTextureData = new TextureData(false, 0, secondHeader[0], secondHeader[1], secondHeader[2], secondHeader[3], secondHeader[4], nullptr);

		bytes = ourMaterial->m_SpecularTextureData->bpp * ourMaterial->m_SpecularTextureData->width * ourMaterial->m_SpecularTextureData->height;
		ourMaterial->m_SpecularTextureData->data = new byte[bytes];
		memcpy(ourMaterial->m_SpecularTextureData->data, cursor, bytes);
		cursor += bytes;
	}
}
