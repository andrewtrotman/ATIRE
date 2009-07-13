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

public:
	virtual char **do_segmentation(char *c);
};

inline char **ANT_plugin::do_segmentation(char *c) { return 0; }

typedef ANT_plugin *maker_t();

struct ANT_plugin_maker
{
	ANT_plugin		*plugin;
	maker_t			*maker;

	void			*dlib; // dynamic library handle loaded by name
};

extern ANT_plugin_maker **plugin_factory;

#endif __PLUGIN_H__
