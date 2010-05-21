/*
 * UNISEG_PLUGIN.H
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#ifndef __UNISEG_PLUGIN_H__
#define __UNISEG_PLUGIN_H__

#include "uniseg_types.h"
#include "seg2.h"
#include <string>

class UNISEG_uniseg : public uniseg_plugin_interface
{
private:
	Seger2				seger_;
	std::string			output_;
	std::string			input_;
	long				count_;
	UNISEG_encoding 	*enc_;

private:
	void cleanup();

public:
	UNISEG_uniseg();
	~UNISEG_uniseg();

	const unsigned char *do_segmentation(unsigned char *c, int length);
	const unsigned char* get_input();
	const unsigned char* get_output();
	int get_count();
};

#endif /* __UNISEG_PLUGIN_H__ */
