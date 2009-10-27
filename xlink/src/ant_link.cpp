/*
 * ant_link.cpp
 *
 *  Created on: Aug 11, 2009
 *      Author: monfee
 */

#include "ant_link.h"
#include "ant_link_term.h"

#include <string.h>
#include <stdlib.h>

/*
	ANT_LINK::COMPARE()
	-------------------
*/
bool ANT_link::compare(const ANT_link *one, const ANT_link *two)
{
//ANT_link *one = NULL, *two = NULL;
double diff;
long cmp;

//one = (ANT_link *)a;
//two = (ANT_link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return -1;
else if (diff > 0)
	return 1;
else
	{
	if ((cmp = strcmp(one->term, two->term)) == 0)
		return one->place_in_file > two->place_in_file ? 1 : one->place_in_file == two->place_in_file ? 0 : -1;
	else
		return cmp;
	}
}

/*
	ANT_LINK::FINAL_COMPARE()
	-------------------------
*/
bool ANT_link::final_compare(const ANT_link *one, const ANT_link *two)
{
//ANT_link *one = NULL, *two = NULL;
double diff;

//one = (ANT_link *)a;
//two = (ANT_link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return -1;
else if (diff > 0)
	return 1;
else
	return one->place_in_file > two->place_in_file ? 1 : one->place_in_file == two->place_in_file ? 0 : -1;
}

/*
	ANT_LINK::STRING_TARGET_COMPARE()
	---------------------------------
*/
bool ANT_link::string_target_compare(const ANT_link *one, const ANT_link *two)
{
//ANT_link *one = NULL, *two = NULL;
int cmp;

//one = (ANT_link *)a;
//two = (ANT_link *)b;

if ((cmp = strcmp(one->term, two->term)) == 0)
	cmp = one->target_document - two->target_document;

return cmp;
}
