/*
	PLUGIN_MANAGER.C
	----------------
	Created on: Jul 10, 2009
	Author: monfee
*/

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "plugin_manager.h"

char ANT_plugin_manager::PLUGIN_DIRECTORY_NAME[] = { "plugins" };
int ANT_plugin_manager::plugin_ids[] = { ANT_plugin::SEGMENTATION };
int ANT_plugin_manager::num_of_plugins = sizeof(ANT_plugin_manager::plugin_ids)/sizeof(int);

#ifdef _MSC_VER
	char *ANT_plugin_manager::plugin_names[] = { "uniseg.dll" };
#else
	char *ANT_plugin_manager::plugin_names[] = { "libuniseg.so" };
#endif


/* global plugins */
ANT_plugin_maker *plugin_factory = NULL;
bool ANT_plugin_manager::loaded = false;


/*
	ANT_PLUGIN_MANAGER::ANT_PLUGIN_MANAGER()
	----------------------------------------
*/
ANT_plugin_manager::ANT_plugin_manager()
{
if (!plugin_factory)
	{
	plugin_factory = new ANT_plugin_maker [num_of_plugins];

	for (int i = 0; i < num_of_plugins; i++)
		{
		plugin_factory[i].plugin = NULL;
		plugin_factory[i].dlib = NULL;
		}
	}
}

/*
	ANT_PLUGIN_MANAGER::~ANT_PLUGIN_MANAGER()
	-----------------------------------------
*/
ANT_plugin_manager::~ANT_plugin_manager()
{
for (int i = 0; i < num_of_plugins; i++)
	{
	if (plugin_factory[i].plugin)
		delete plugin_factory[i].plugin;

#ifdef _MSC_VER
	// close the dll here
#else
	if (plugin_factory[i].dlib)
		dlclose(plugin_factory[i].dlib);
#endif
	}
delete [] plugin_factory;
}

/*
	ANT_PLUGIN_MANAGER::LOAD_LIBRARY()
	----------------------------------
*/
#ifdef _MSC_VER
void ANT_plugin_manager::load_library(const char *library_file, int id)
{
/*
	please help with implementation of loading dynamic library on Windows
*/
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

#else
void ANT_plugin_manager::load_library(const char *library_file, int id)
{
plugin_factory[plugin_ids[id]].dlib = dlopen(library_file, RTLD_NOW);
if (plugin_factory[plugin_ids[id]].dlib == NULL )
	printf("opening plugin(%s) failed: %s\n", library_file, dlerror());
else
	{
	fprintf(stderr, "found plugin(%s)\n", library_file);
	if (plugin_factory[plugin_ids[id]].maker == NULL)
		{
		fprintf(stderr, "this library(%s) wasn't made as a ANT plugin, please make sure WITH_ANT_PLUGIN macro is defined\n", library_file);
		return;
		}
	plugin_factory[plugin_ids[id]].plugin = plugin_factory[plugin_ids[id]].maker();
	}
}
#endif

/*
	ANT_PLUGIN_MANAGER::LOAD()
	--------------------------
*/
void ANT_plugin_manager::load()
{
if (loaded)
    return;

char *name = 0;

for (int i = 0; i < num_of_plugins; i++)
	{
	name = plugin_names[i];
	#ifdef _MSC_VER
		char sep[] = { "\\" };
	#else
		char sep[] = { "/" };
	#endif
	size_t len = strlen(PLUGIN_DIRECTORY_NAME) + strlen(sep) + strlen(name);
	char *name_with_plugin_path = new char[len + 1];
	strcpy(name_with_plugin_path, PLUGIN_DIRECTORY_NAME);
	strcat(name_with_plugin_path, sep);
	strcat(name_with_plugin_path, name);
	name_with_plugin_path[len] = '\0';
	struct stat plugin_stat;
	if (stat( name_with_plugin_path, &plugin_stat ) != -1)
		load_library(name_with_plugin_path, i);
	else
		load_library(name, i);
//	else
//		fprintf(stderr, "no plugin found for : %s\n", name_with_plugin_path);
	delete [] name_with_plugin_path;
	}

loaded = true;
}

/*
	ANT_PLUGIN_MANAGER::DO_SEGMENTATION()
	-------------------------------------
*/
const unsigned char *ANT_plugin_manager::do_segmentation(unsigned char *c, int length)
{
if (plugin_factory[ANT_plugin::SEGMENTATION].plugin)
	return plugin_factory[ANT_plugin::SEGMENTATION].plugin->do_segmentation(c, length);
return 0;
}
