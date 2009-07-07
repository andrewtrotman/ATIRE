/*
 * UNISEQ_SETTINGS.C
 * -----------------
 *
 *  Created on: Jul 6, 2009
 *      Author: monfee
 */

#include "uniseg_settings.h"

UNISEG_settings::UNISEG_settings()
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

lang = uniseg_encoding::UNKNOWN;

reward = false;
mean = 1;
mi = 0;

max = 5;

#ifdef _MSC_VER
	sep[0] = '\\';
#else
	sep[0] = '/';
#endif

sep[1] = '\0';
}

UNISEG_settings& UNISEG_settings::instance() {
	static UNISEG_settings inst;
	return inst;
}

bool UNISEG_settings::skipit(int size, int freq) {
	if (skip.size() <= 0)
		return false;

	std::vector<int>::iterator ret = std::find(skip.begin(), skip.end(), size);
	if (ret != skip.end()) {
		if (skip_low == 0 && skip_high != 0) {
			if (freq > skip_high)
				return true;
		}
		else if (skip_high == 0 && skip_low != 0) {
			if (freq < skip_low)
				return true;
		}
		else {
			if (freq > skip_low && freq < skip_high)
				return true;
		}
	}
	return false;
}
