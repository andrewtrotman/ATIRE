/*
 * UNISEG_PLUGIN.H
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#ifndef __UNISEG_PLUGIN_H__
#define __UNISEG_PLUGIN_H__

#include "uniseg_types.h"
#include "seg.h"

class UNISEG_plugin : public uniseg_plugin_interface
{
private:
	Seger	seger;
	unsigned char	**output;

public:
	UNISEG_plugin();
	~UNISEG_plugin();

	int do_segmentation(unsigned char *c, unsigned char **out);
};

#endif /* __UNISEG_PLUGIN_H__ */
