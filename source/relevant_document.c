/*
	RELEVANT_DOCUMENT.C
	-------------------
*/

#include "relevant_document.h"

/*
	ANT_RELEVANT_DOCUMENT::COMPARE()
	--------------------------------
*/
int ANT_relevant_document::compare(const void *a, const void *b)
{
ANT_relevant_document *one, *two;

one = (ANT_relevant_document *)a;
two = (ANT_relevant_document *)b;

if (two->topic == one->topic)						// sort on topic first
	return two->docid < one->docid ? -1 : two->docid == one->docid ? 0 : 1;		// then on docid
else
	return two->topic < one->topic ? -1 : 1;
}

