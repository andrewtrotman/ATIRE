/*
 * UNISEQ_SETTINGS.H
 * -----------------
 *
 *  Created on: Jul 6, 2009
 *      Author: monfee
 */

#ifndef __UNISEQ_SETTINGS_H__
#define __UNISEQ_SETTINGS_H__

#include "uniseg_types.h"

class UNISEQ_settings
{
public:
	static const int MAX_CHARS = 100;

public:
	bool 					load;
	bool 					verbose;
	bool					debug;

	std::vector<int> 		skip;
	int 					skip_high;
	int 					skip_low;
	int 					to_skip; // for all skipping

	int 					max;

	uniseg_encoding::language 	lang;

	std::string 			wd;
	char  					sep[2];


	int 					mean;
	int 					mi;
	bool 					reward;
	bool					optimize;

public:
	UNISEQ_settings();
	~UNISEQ_settings() {}

	static UNISEQ_settings& instance();

	bool skipit(int size, int freq);
};
#endif /* __UNISEQ_SETTINGS_H__ */
