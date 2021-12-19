#include "MaterialImporter.h"

#include "Application.h"
#include "ModuleTexture.h"

#include "assimp/ai_assert.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

#include "GL/glew.h"

void importer::material::Import(const aiMaterial* material, Texture* ourMaterial, const char* fullPath)
{
	aiString file;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &file) != AI_SUCCESS)
		return;

	ourMaterial->m_Name = new char[strlen(file.data) + 1];
	strcpy(ourMaterial->m_Name, file.data);

	ourMaterial->m_MinFilter = GL_LINEAR;
	ourMaterial->m_MagFilter = GL_LINEAR;
	ourMaterial->m_Wrap = GL_CLAMP;

	glGenTextures(1, &ourMaterial->m_Texture);
	glBindTexture(GL_TEXTURE_2D, ourMaterial->m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ourMaterial->m_MinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ourMaterial->m_MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ourMaterial->m_Wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ourMaterial->m_Wrap);

	MY_LOG("Assimp texture (%s): Loading model from the path described in the FBX", file.data);
	if (!App->texture->LoadTextureData(file.data, ourMaterial->width, ourMaterial->height, ourMaterial->depth, ourMaterial->format))
	{
		std::string directoryPath = fullPath;
		const size_t last_slash_idx = directoryPath.rfind('\\');
		if (std::string::npos != last_slash_idx)
			directoryPath = directoryPath.substr(0, last_slash_idx) + '\\' + file.data;

		MY_LOG("Assimp texture (%s): Loading model from the same folder you of the FBX", directoryPath.c_str());
		if (!App->texture->LoadTextureData(directoryPath.c_str(), ourMaterial->width, ourMaterial->height, ourMaterial->depth, ourMaterial->format))
		{
			directoryPath = TEXTURES_FOLDER;
			directoryPath += file.data;

			MY_LOG("Assimp texture (%s): Loading model from the Textures/ folder", directoryPath.c_str());
			if (!App->texture->LoadTextureData(directoryPath.c_str(), ourMaterial->width, ourMaterial->height, ourMaterial->depth, ourMaterial->format))
			{
				MY_LOG("Assimp texture: Texture couldn't be loaded");
				return;
			}
		}
	}

	MY_LOG("Assimp texture: Texture loaded correctly");
}

int importer::material::Save(const Texture* ourMaterial, char*& fileBuffer)
{
	unsigned int header[7] = {
		ourMaterial->width,
		ourMaterial->height,
		ourMaterial->depth,
		ourMaterial->format,
		ourMaterial->m_MinFilter,
		ourMaterial->m_MagFilter,
		ourMaterial->m_Wrap
	};

	unsigned int size = sizeof(header) + sizeof(byte) * 3 * ourMaterial->width * ourMaterial->height;

	// Allocate
	fileBuffer = new char[size];
	char* cursor = fileBuffer;

	// Store header
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, sizeof(header));
	cursor += bytes;

	// Store vertices
	bytes = sizeof(byte) * 3 * ourMaterial->width * ourMaterial->height;
	memcpy(cursor, ourMaterial->MapTextureBuffer(), bytes);
	ourMaterial->UnMapBuffer();
	cursor += bytes;

	return size;
}

void importer::material::Load(const char* fileBuffer, Texture* ourMaterial)
{
	const char* cursor = fileBuffer;

	MY_LOG("MaterialImporter_Load: Reading main variables");
	unsigned int header[7];
	unsigned int bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourMaterial->width = header[0];
	ourMaterial->height = header[1];
	ourMaterial->depth = header[2];
	ourMaterial->format = header[3];
	ourMaterial->m_MinFilter = header[4];
	ourMaterial->m_MagFilter = header[5];
	ourMaterial->m_Wrap = header[6];

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

	unsigned int buffer_size = sizeof(byte) * 3 * ourMaterial->width * ourMaterial->height;
	glBufferData(GL_TEXTURE_2D, buffer_size, nullptr, GL_STATIC_DRAW);

	unsigned int* pointer = (unsigned int*)(glMapBufferRange(GL_TEXTURE_2D, 0, buffer_size, GL_MAP_WRITE_BIT));

	bytes = buffer_size;
	memcpy(pointer, cursor, bytes);
	cursor += bytes;

	glUnmapBuffer(GL_TEXTURE_2D);

	glGenerateTextureMipmap(ourMaterial->m_Texture);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}
