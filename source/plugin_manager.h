/*
 * PLUGIN_MANAGER.H
 * ----------------
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#ifndef PLUGIN_MANAGER_H_
#define PLUGIN_MANAGER_H_

#include "plugin.h"

/*
	class ANT_PLUGIN_MANAGER
	------------------------
*/
class ANT_plugin_manager
{

private:
	static bool				loaded;

public:
	static char 			PLUGIN_DIRECTORY_NAME[];

	static int				plugin_ids[];  // registered plugin ids
	static int				num_of_plugins;
	static char				*plugin_names[]; // plugin dynamic library name array, e.g segmentation module on linux named libuniseg.so, on Windows named uniseg.dll

private:
	void load_library(const char *library_file, int id);

public:
	ANT_plugin_manager();
	~ANT_plugin_manager();

	static ANT_plugin_manager& instance();

	const unsigned char *do_segmentation(unsigned char *c, int length);
	ANT_plugin *segmentation_plugin();
	inline bool is_segmentation_plugin_available();

	void load();
};

/*
	ANT_PLUGIN_MANAGER::INSTANCE()
	------------------------------
*/
inline ANT_plugin_manager& ANT_plugin_manager::instance()
{
static ANT_plugin_manager inst;

if (!loaded)
    inst.load();

return inst;
}


/*
	ANT_PLUGIN_MANAGER::SEGMENTATION_PLUGIN()
	-----------------------------------------
*/
inline ANT_plugin *ANT_plugin_manager::segmentation_plugin()
{
return plugin_factory[ANT_plugin::SEGMENTATION].plugin;
}

/*
	ANT_PLUGIN_MANAGER::IS_SEGMENTATION_PLUGIN_AVAILABLE()
	------------------------------------------------------
*/
inline bool ANT_plugin_manager::is_segmentation_plugin_available()
{
return segmentation_plugin() != NULL;
}

#endif  /* PLUGIN_MANAGER_H_ */
