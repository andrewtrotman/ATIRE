/*
 * PLUGIN_MANAGER.H
 * ----------------
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

#include "plugin.h"

class ANT_plugin_segmentation;

class ANT_plugin_manager
{
public:
	static int		plugin_ids[];  // registered plugin ids
	ANT_plugin		**plugins;
	int				num_of_plugins;

public:
	ANT_plugin_manager();
	~ANT_plugin_manager();

	static ANT_plugin_manager& instance();
	void plug(ANT_plugin::plugin_type);
	void load();

	// the reason with using a plugin function is that we don't have unified interfaces for achieving particular tasks
	ANT_plugin_segmentation *get_segmentation_plugin();
};

inline ANT_plugin_manager& instance()
{
static ANT_plugin_manager inst;
return inst;
}

#endif /* __PLUGIN_MANAGER_H__ */
