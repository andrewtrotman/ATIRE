/*
	RELEVANT_DOCUMENT.C
	-------------------
*/
#include <string.h>
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

// sort on topic, subtopic, docid
if (two->topic < one->topic)
	return -1;
if (two->topic > one->topic)
	return 1;
if (two->subtopic < one->subtopic)
	return -1;
if (two->subtopic > one->subtopic)
	return 1;

#ifdef FILENAME_INDEX
	return strcmp(one->docid, two->docid);
#else
	if (two->docid < one->docid)
		return -1;
	if (two->docid > one->docid)
		return 1;

	return 0;
#endif
}
