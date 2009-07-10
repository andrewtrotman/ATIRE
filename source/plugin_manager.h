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

class ANT_plugin_manager
{
public:
	static int		plugin_ids[];  // registered plugin ids
	ANT_plugin		*plugins;

public:
	ANT_plugin_manager();
	~ANT_plugin_manager();

	static ANT_plugin_manager& instance();
	void load(ANT_plugin::plugin_type);
};

inline ANT_plugin_manager& instance()
{
static ANT_plugin_manager inst;
return inst;
}

#endif /* __PLUGIN_MANAGER_H__ */
