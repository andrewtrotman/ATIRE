/*
 * UNISEQ_SETTINGS.C
 * -----------------
 *
 *  Created on: Jul 6, 2009
 *      Author: monfee
 */

#include "uniseg_settings.h"

LIB_uniseg_settings::LIB_uniseg_settings()
{
#ifdef DEBUG
	debug = true;
#else
	debug = false;
#endif

verbose = true;
load = true;

to_skip = 1;
skip_high = 0;
skip_low = 0;

lang = stpl::UNKNOWN;
format = XML;

reward = false;
mean = 1;
mi = 0;

max = 5;

#ifdef _MSC_VER
	sep_[0] = '\';
#else
	sep_[0] = '/';

sep_[1] = '\0';
}

