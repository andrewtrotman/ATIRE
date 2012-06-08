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

one = (ANT_relevant_topic *)a;
two = (ANT_relevant_topic *)b;

// sort on topic
if (two->topic < one->topic) return -1;
if (two->topic > one->topic) return 1;
return 0;
}

