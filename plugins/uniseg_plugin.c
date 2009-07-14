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
	output = 0;

	// load the string frequency table
	seger.load_frqs();
}

UNISEG_plugin::~UNISEG_plugin()
{
}

const char *UNISEG_plugin::do_segmentation(unsigned char *c)
{
	seger.input(c);
	seger.start();
	return seger.output();
}

const char* UNISEG_plugin::get_input()
{
return seger.stream().c_str();
}

const char* UNISEG_plugin::get_output()
{
return seger.stream_out().c_str();
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
		   plugin_factory[uniseg_plugin_interface::SEGMENTATION]->maker = maker;
	   }
	};
	// our one instance of the proxy
	proxy p;
}
