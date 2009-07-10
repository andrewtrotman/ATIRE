/*
 * PLUGIN_MANAGER.C
 * ----------------
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include "plugin_manager.h"

int	ANT_plugin_manager::plugin_ids[] = { ANT_plugin::SEGMENTATION };

ANT_plugin_manager::ANT_plugin_manager()
{
plugins = new ANT_plugin[sizeof(plugin_ids)/sizeof(int)];
}

ANT_plugin_manager::~ANT_plugin_manager()
{

}

void ANT_plugin_manager::load(ANT_plugin::plugin_type plugin_type)
{
//if ()
}
