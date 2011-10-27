/*
	RELEVANT_TOPIC.C
	----------------
*/

#include "relevant_topic.h"

/*
	ANT_RELEVANT_TOPIC::COMPARE()
	-----------------------------
*/
int ANT_relevant_topic::compare(const void *a, const void *b)
{
ANT_relevant_topic *one, *two;
long long diff;

one = (ANT_relevant_topic *)a;
two = (ANT_relevant_topic *)b;

diff = two->topic - one->topic;
return diff > 0 ? 1 : diff < 0 ? -1 : 0;
}

