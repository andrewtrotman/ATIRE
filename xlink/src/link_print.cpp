/*
 * link_print.cpp
 *
 *  Created on: Oct 28, 2010
 *      Author: monfee
 */

#include "link_print.h"


#ifdef CROSSLINK
	std::string QLINK::link_print::target_format = "\t\t\t\t<tofile bep_offset=\"%d\" lang=\"%s\" title=\"%s\">%d</tofile>\n";
#else
	std::string QLINK::link_print::target_format = "\t\t\t\t<tobep offset=\"%d\">%d</tobep>\n";
#endif
