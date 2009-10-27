/*
 * ant_link_posting.cpp
 *
 *  Created on: Oct 14, 2009
 *      Author: monfee
 */

#include "ant_link_posting.h"
#include <stdlib.h>

ANT_link_posting::ANT_link_posting()
{
	docid = -1;
	offset = 0;
	link_frequency = 0;
	doc_link_frequency = 0;
	desc = NULL;
}

ANT_link_posting::~ANT_link_posting()
{
	if (desc)
		free(desc);
}

/*
	ANT_LINK_POSTING::COMPARE()
	---------------------------
*/
inline bool ANT_link_posting::compare(const ANT_link_posting *a, const ANT_link_posting *b)
{
ANT_link_posting *one = NULL, *two = NULL;

one = (ANT_link_posting *)a;
two = (ANT_link_posting *)b;

return two->link_frequency > one->link_frequency; // ? 1 : two->link_frequency == one->link_frequency ? 0 : -1;
}
