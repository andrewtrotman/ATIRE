/*
 * PLUGIN.H
 * --------
 *
 *  Created on: Jul 2, 2009
 *      Author: monfee
 */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

/*
	class ANT_PLUGIN
	----------------
*/
class ANT_plugin
{
public:
	enum plugin_id { SEGMENTATION  = 0 };

protected:
	char			*name;	// plugin library name, e.g segmentation module on linux named libuniseg.so, on Windows named uniseg.dll
	ANT_plugin		*plugin;
	ANT_plugin		*maker();

public:
	ANT_plugin();
	~ANT_plugin();

	virtual char **do_segmentation(char *c);
};

inline char **ANT_plugin::do_segmentation(char *c) { return 0; }

struct ANT_plugin_maker
{
	ANT_plugin		*plugin;
	ANT_plugin		*maker();

	void			*dlib; // dynamic library handle loaded by name
};

extern ANT_plugin_maker *plugin_factory;

#endif __PLUGIN_H__
