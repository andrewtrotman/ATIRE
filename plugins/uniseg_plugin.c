/*
 * UNISEG_PLUGIN.C
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include "uniseg_plugin.h"
#include "seg.h"

UNISEG_plugin::UNISEG_plugin() : uniseg_plugin_interface()
{
	name = SEGMENTATION_PLUGIN_NAME;
	output = 0;
}

UNISEG_plugin::~UNISEG_plugin()
{
}

char **UNISEG_plugin::do_segmentation(char *c)
{
	if (!dlib)
		return 0;

	seger.input(c);
	seger.start();

	output = seger.output();
}

extern "C"
{
	uniseg_plugin_interface *maker() {
	   return new UNISEG_plugin;
	}

	class proxy
	{
	public:
	   proxy() {
		  // register the maker with the factory
		   plugins[uniseg_plugin_interface::SEGMENTATION] = maker;
	   }
	};
	// our one instance of the proxy
	proxy p;
}
