/*
	INDEX_DOCUMENT_GLOBAL_STATS.H
	-----------------------------
*/
#ifndef INDEX_DOCUMENT_GLOBAL_STATS_H_
#define INDEX_DOCUMENT_GLOBAL_STATS_H_

#include "string_pair.h"

/*
	class ANT_INDEX_DOCUMENT_GLOBAL_STATS
	-------------------------------------
*/
class ANT_index_document_global_stats
{
public:
	ANT_index_document_global_stats *left, *right;
	ANT_string_pair term;
	long long collection_frequency;
	long long document_frequency;

public:
	ANT_index_document_global_stats(ANT_string_pair *string)	{ left = right = 0; term.start = string->start; term.string_length = string->string_length; collection_frequency = 1; }
	void *operator new (size_t bytes, ANT_memory *memory) { (void)memory; return ::new char [bytes]; }
} ;

#endif /* INDEX_DOCUMENT_GLOBAL_STATS_H_ */
