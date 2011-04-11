/*
 * ant_link_term.cpp
 *
 *  Created on: Sep 22, 2009
 *      Author: monfee
 */

#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "ant_link_parts.h"

#include <string.h>
#include <stdlib.h>

ANT_link_term::ANT_link_term()
{
	term = NULL;
	//postings = NULL;
	//postings_length = 0;
	collection_frequency = 0;
	document_frequency = 0;
	document_frequency = 0;
}

ANT_link_term::~ANT_link_term()
{
	if (term) {
		free(term);
		term = NULL;
	}
	//if (postings) {
		for (int i = 0; i < postings.size(); i++)
			delete postings[i];
//		delete [] postings;
//		postings = NULL;
	//}
}

/*
	ANT_LINK_TERM::COMPARE()
	------------------------
*/
int ANT_link_term::compare(const void *a, const void *b)
{
ANT_link_term *one = NULL, *two = NULL;

one = (ANT_link_term *)a;
two = (ANT_link_term *)b;

if (one->term == NULL) {
	if (two->term == NULL)
		return 0;
	else
		return -1;
} else if (two->term == NULL)
	return 1;

return strcmp(one->term, two->term);
}

bool ANT_link_term::operator==(const ANT_link_term& rhs)
{
	return strcmp(rhs.term, this->term) == 0;
}

bool ANT_link_term::operator==(const char *term)
{
	int cmp = strcmp(this->term, term);
	return cmp == 0;
}

const bool ANT_link_term_compare::operator()(const ANT_link_term *one, const ANT_link_term *two) const
{
int cmp, min_len;

char *new_one = strdup(one->term);
char *new_two = strdup(two->term);

if (strchr(new_one, ' ') != NULL)
	string_remove_space(new_one);

if (strchr(new_two, ' ') != NULL)
	string_remove_space(new_two);

min_len = MIN(strlen(new_one), strlen(new_two));

cmp = memcmp(new_one, new_two, min_len);

if (cmp == 0)
	{
	if (strlen(new_one) < strlen(new_two))
		cmp = -1;
	else if (strlen(new_one) > strlen(new_two))
		cmp = 1;
	}

free(new_one);
free(new_two);

return cmp;
}
