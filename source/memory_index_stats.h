/*
	MEMORY_INDEX_STATS.H
	--------------------
*/

#ifndef __MEMORY_INDEX_STATS_H__
#define __MEMORY_INDEX_STATS_H__

class ANT_memory_index_stats
{
public:
	long hash_nodes;
	long unique_terms;
	long long documents;
	long long term_occurences;
	long long bytes_to_store_docids;
	long long bytes_allocated_for_docids;
public:
	ANT_memory_index_stats();
	~ANT_memory_index_stats();

	void render(void);
} ;

#endif __MEMORY_INDEX_STATS_H__
