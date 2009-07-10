/*
 * PLUGIN_MANAGER.C
 * ----------------
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include "plugin_manager.h"
#include "plugin_segmentation.h"

int	ANT_plugin_manager::plugin_ids[] = { ANT_plugin::SEGMENTATION };

ANT_plugin_manager::ANT_plugin_manager()
{
num_of_plugins = sizeof(plugin_ids)/sizeof(int);
plugins = new ANT_plugin*[num_of_plugins];

for (int i = 0; i < num_of_plugins; i++)
	plugins[i] = NULL;
}

ANT_plugin_manager::~ANT_plugin_manager()
{
delete [] plugins;
}

void ANT_plugin_manager::plug(ANT_plugin::plugin_type plugin_type)
{
switch (plugin_type)
	{
	case ANT_plugin::SEGMENTATION:
		plugins[ANT_plugin::SEGMENTATION] = new ANT_plugin_segmentation;
		break;
	default:
		break;
	}
}

void ANT_plugin_manager::load()
{
	for (int i = 0; i < num_of_plugins; i++)
		plugins[i]->load();
}

ANT_plugin_segmentation* ANT_plugin_manager::get_segmentation_plugin()
{
return reinterpret_cast<ANT_plugin_segmentation*>(plugins[ANT_plugin::SEGMENTATION]);
}
