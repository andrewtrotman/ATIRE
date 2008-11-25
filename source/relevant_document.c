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
int cmp;

one = (ANT_relevant_document *)a;
two = (ANT_relevant_document *)b;

if ((cmp = two->topic - one->topic) != 0)			// sort on topic first
	return cmp;

return two->docid - one->docid;					// then on document id
}

