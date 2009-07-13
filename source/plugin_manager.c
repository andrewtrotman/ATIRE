/*
 * PLUGIN_MANAGER.C
 * ----------------
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "plugin_manager.h"

char ANT_plugin_manager::PLUGIN_DIRECTORY_NAME[] = { "plugins" };

int	ANT_plugin_manager::plugin_ids[] = { ANT_plugin::SEGMENTATION };

int ANT_plugin_manager::num_of_plugins = sizeof(ANT_plugin_manager::plugin_ids)/sizeof(int);


#ifdef _MSC_VER
	char ANT_plugin_manager::plugin_names[][] = { { "uniseg.dll" } {} };
#else
	char ANT_plugin_manager::plugin_names[][] = { { "libuniseg.so"} {} };
#endif


/* global plugins */
ANT_plugin_maker *plugin_factory = NULL;

ANT_plugin_manager::ANT_plugin_manager()
{
if (!plugin_factory)
	{
	plugin_factory = new ANT_plugin_maker[num_of_plugins];

	for (int i = 0; i < num_of_plugins; i++)
		{
		plugin_factory[i]->plugin = NULL;
		plugin_factory[i]->dlib = NULL;
		}
	}
}

ANT_plugin_manager::~ANT_plugin_manager()
{
for (int i = 0; i < num_of_plugins; i++)
	if (plugin_factory[i]->plugin)
		delete plugin_factory[i]->plugin;
	if (plugin_factory[i]->dlib)
		dlclose(dlib);

delete [] plugin_factory;
}

void ANT_plugin_manager::load()
{
char *name = 0;

for (int i = 0; i < num_of_plugins; i++)
	{
	name = plugin_names[i];
	#ifdef _MSC_VER
		char sep[] = { "\\" };
	#else
		char sep[] = { "/" };
	#endif
	char *name_with_plugin_path = new char[strlen(PLUGIN_DIRECTORY_NAME) + strlen(sep) + strlen(name)];
	strcpy(name_with_plugin_path, PLUGIN_DIRECTORY_NAME);
	strcat(name_with_plugin_path, sep);
	strcat(name_with_plugin_path, name);
	dlib = dlopen(name_with_plugin_path, RTLD_NOW);
	if (dlib == NULL )
		printf("opening plugin(%s) failed: %s\n", name_with_plugin_path, dlerror());
	delete name_with_plugin_path;
	}
}

char **ANT_plugin_manager::do_segmentation(char *c)
{
return plugins[ANT_plugin::SEGMENTATION]->do_segmentation(c);
}
