/*
 * UNISEG_PLUGIN.C
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include "uniseg_plugin.h"
#include "seg.h"
#include <iostream>

using namespace std;

UNISEG_plugin::UNISEG_plugin() : uniseg_plugin_interface()
{
	output = 0;

	// load the string frequency table
	cout << "##################### initializing UNISEG segmentation module ######################" << endl;
	seger.load_frqs();
	cout << "############################# finished initialization ##############################" << endl;
}

UNISEG_plugin::~UNISEG_plugin()
{
}

const unsigned char *UNISEG_plugin::do_segmentation(unsigned char *c, int length)
{
	if (!c || strlen((char *)c) == 0 || length == 0)
		return NULL;

	seger.input(c, length);
	seger.start();
	return seger.output();
}

const unsigned char *UNISEG_plugin::get_input()
{
return (unsigned char *)seger.stream().c_str();
}

const unsigned char *UNISEG_plugin::get_output()
{
return (unsigned char *)seger.stream_out().c_str();
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
