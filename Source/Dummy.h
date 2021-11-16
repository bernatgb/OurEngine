#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleDummy : public Module
{
	bool Init()
	{
		MY_LOG("Dummy Init!");
		return true;
	}

	bool CleanUp()
	{
		MY_LOG("Dummy CleanUp!");
		return true;
	}
};