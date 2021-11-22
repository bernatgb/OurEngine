#pragma once
#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"

#include <string.h>

#include <stdarg.h>
#include <debugapi.h>

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	const char* nameFile = strrchr(file, '\\') ? strrchr(file, '\\') + 1 : file;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	//sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", nameFile, line, tmp_string);
	OutputDebugString(tmp_string2);

	//Console
	int length = strlen(tmp_string2);
	char* str = (char*)malloc(length + 1);
	memcpy(str, tmp_string2, length);
	str[length] = '\0';

	if (App != nullptr && App->imGui != nullptr)
		App->imGui->Items.push_back(str);
}