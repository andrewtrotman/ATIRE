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
	static char 			PLUGIN_DIRECTORY_NAME[];

	static int				plugin_ids[];  // registered plugin ids
	static int				num_of_plugins;
	static char				*plugin_names[]; // plugin dynamic library name array, e.g segmentation module on linux named libuniseg.so, on Windows named uniseg.dll

public:
	ANT_plugin_manager();
	~ANT_plugin_manager();

	static ANT_plugin_manager& instance();
//	void plug(ANT_plugin::plugin_type);
	void load();

	int do_segmentation(unsigned char *c, unsigned char **out);
};

inline ANT_plugin_manager& ANT_plugin_manager::instance()
{
static ANT_plugin_manager inst;
return inst;
}

#endif /* __PLUGIN_MANAGER_H__ */
