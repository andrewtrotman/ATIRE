/*
	SEARCH_ENGINE_STATS.H
	---------------------
*/
#ifndef __SEARCH_ENGINE_STATS_H__
#define __SEARCH_ENGINE_STATS_H__

#include "time_stats.h"

/*
	class ANT_SEARCH_ENGINE_STATS
	-----------------------------
*/
class ANT_search_engine_stats : public ANT_time_stats
{
public:
	long long decompress_time;
	long long posting_read_time;
	long long rank_time;
	long long accumulator_init_time;
	long long sort_time;
	long long count_relevant_time;
	long long dictionary_time;
	long long stemming_time;

public:
	ANT_search_engine_stats(ANT_memory *memory) ;
	virtual ~ANT_search_engine_stats() {}
	virtual void text_render(void);

	void initialise(void);
	void add(ANT_search_engine_stats *what);

	void add_posting_read_time(long long time) { posting_read_time += time; }
	void add_decompress_time(long long time) { decompress_time += time; }
	void add_rank_time(long long time) { rank_time += time; }
	void add_accumulator_init_time(long long time) { accumulator_init_time += time; }
	void add_sort_time(long long time) { sort_time += time; } 
	void add_count_relevant_documents(long long time) { count_relevant_time += time; }
	void add_dictionary_lookup_time(long long time) { dictionary_time += time; }
	void add_stemming_time(long long time) { stemming_time += time; }
} ;

#endif __SEARCH_ENGINE_STATS_H__
