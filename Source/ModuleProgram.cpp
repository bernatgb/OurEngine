#include "Globals.h"
#include "ModuleProgram.h"
#include "GL/glew.h"

#include <stdlib.h>

char* LoadShaderSource(const char* shader_file_name)
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, shader_file_name, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}

	return data;
}

unsigned CompileShader(unsigned type, const char* name, const char* source)
{
	unsigned shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, 0);
	glCompileShader(shader_id);

	int res = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetShaderInfoLog(shader_id, len, &written, info);
			MY_LOG("Log Info (%s): %s", name, info);
			free(info);
		}
	}
	return shader_id;
}

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

unsigned int ModuleProgram::CreateProgram(const char* vtx_shader_file_name, const char* frg_shader_file_name) const
{
	unsigned vtx_shader = CompileShader(GL_VERTEX_SHADER, vtx_shader_file_name, LoadShaderSource(vtx_shader_file_name));
	unsigned frg_shader = CompileShader(GL_FRAGMENT_SHADER, frg_shader_file_name, LoadShaderSource(frg_shader_file_name));

	unsigned int program = glCreateProgram();
	glAttachShader(program, vtx_shader);
	glAttachShader(program, frg_shader);
	glLinkProgram(program);
	int res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*)malloc(len);
			glGetProgramInfoLog(program, len, &written, info);
			MY_LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vtx_shader);
	glDeleteShader(frg_shader);

	return program;
}
