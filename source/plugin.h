/*
 * PLUGIN.H
 * --------
 *
 *  Created on: Jul 2, 2009
 *      Author: monfee
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "pragma.h"

/*
	class ANT_PLUGIN
	----------------
*/
class ANT_plugin
{
public:
	enum plugin_id { SEGMENTATION  = 0 };

public:
	virtual ~ANT_plugin() {}
	virtual const unsigned char *do_segmentation(unsigned char *c, int length);
	virtual const unsigned char *get_input();
	virtual const unsigned char *get_output();
	virtual int get_count();
};

/*
	ANT_PLUGIN::DO_SEGMENTATION()
	-----------------------------
*/
#pragma ANT_PRAGMA_UNUSED_PARAMETER
inline const unsigned char *ANT_plugin::do_segmentation(unsigned char *c, int length) { return 0; }

/*
	ANT_PLUGIN::GET_INPUT()
	-----------------------
*/
inline const unsigned char *ANT_plugin::get_input() { return NULL; }

/*
	ANT_PLUGIN::GET_OUTPUT()
	------------------------
*/
inline const unsigned char *ANT_plugin::get_output() { return NULL; }

/*
	ANT_PLUGIN::GET_COUNT()
	-----------------------
*/
inline int ANT_plugin::get_count() { return 0; }

typedef ANT_plugin *maker_t();

/*
	struct ANT_PLUGIN_MAKER
	-----------------------
*/
struct ANT_plugin_maker
{
	ANT_plugin		*plugin;
	maker_t			*maker;

	void			*dlib; // dynamic library handle loaded by name
};

extern ANT_plugin_maker *plugin_factory;

#endif  /* PLUGIN_H_ */
