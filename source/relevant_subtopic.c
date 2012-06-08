/*
	RELEVANT_SUBTOPIC.C
	-------------------
*/

#include "relevant_subtopic.h"

/*
	ANT_RELEVANT_SUBTOPIC::COMPARE()
	--------------------------------
*/
int ANT_relevant_subtopic::compare(const void *a, const void *b)
{
ANT_relevant_subtopic *one, *two;

one = (ANT_relevant_subtopic *)a;
two = (ANT_relevant_subtopic *)b;

// sort on topic, then subtopic
if (two->topic < one->topic) return -1;
if (two->topic > one->topic) return 1;
if (two->subtopic < one->subtopic) return -1;
if (two->subtopic > one->subtopic) return 1;
return 0;
}

