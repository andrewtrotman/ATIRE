/*
	STATS_MEMORY_INDEX.H
	--------------------
*/
#ifndef STATS_MEMORY_INDEX_H_
#define STATS_MEMORY_INDEX_H_

#include "stats.h"

/*
	class ANT_STATS_MEMORY_INDEX
	----------------------------
*/
class ANT_stats_memory_index : public ANT_stats
{
public:
	enum {STAT_MEMORY = 1, STAT_SUMMARY = 2} ;

public:
	ANT_memory *postings_memory;
	long hash_nodes;
	long unique_terms;
	long long documents;
	long long term_occurences;
	long long bytes_to_store_docids;
	long long bytes_allocated_for_docids;
	long long bytes_to_store_tfs;
	long long bytes_allocated_for_tfs;
	long long bytes_used_to_sort_term_list;
	long long posting_fragments;
	long long strings;
	long long bytes_in_string_pool;
	long long bytes_for_decompression_recompression;
	long disk_buffer;
	long long time_to_quantize;
	long long bytes_to_quantize;
	long long time_to_store_documents_on_disk;
	long long bytes_to_store_documents_on_disk;

public:
	ANT_stats_memory_index(ANT_memory *memory = NULL, ANT_memory *postings_memory = NULL);
	virtual ~ANT_stats_memory_index();

	virtual void text_render(long which_stats);
	using ANT_stats::text_render;
} ;



#endif /* STATS_MEMORY_INDEX_H_ */
