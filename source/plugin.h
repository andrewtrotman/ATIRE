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
	virtual const char *do_segmentation(unsigned char *c);
	virtual const char *get_input();
	virtual const char *get_output();
};

inline const char *ANT_plugin::do_segmentation(unsigned char *c) { return 0; }
inline const char *ANT_plugin::get_input() { return NULL; }
inline const char *ANT_plugin::get_output() { return NULL; }

typedef ANT_plugin *maker_t();

struct ANT_plugin_maker
{
	ANT_plugin		*plugin;
	maker_t			*maker;

	void			*dlib; // dynamic library handle loaded by name
};

extern ANT_plugin_maker **plugin_factory;

#endif __PLUGIN_H__
