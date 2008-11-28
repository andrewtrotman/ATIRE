/*
	MEMORY_INDEX_STATS.H
	--------------------
*/

#ifndef __MEMORY_INDEX_STATS_H__
#define __MEMORY_INDEX_STATS_H__

class ANT_memory;
class ANT_memory_index_stats
{
public:
	ANT_memory *memory;
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
	long disk_buffer;
public:
	ANT_memory_index_stats(ANT_memory *memory = NULL);
	~ANT_memory_index_stats();

	void text_render(void);
	long long get_clock_tick(void);
	long long get_clock_tick_frequency(void);
} ;

#endif /* __MEMORY_INDEX_STATS_H__ */
