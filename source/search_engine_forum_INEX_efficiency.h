/*
	SEARCH_ENGINE_FORUM_INEX_EFFICIENCY.H
	-------------------------------------
*/
#ifndef SEARCH_ENGINE_FORUM_INEX_EFFICIENCY_H_
#define SEARCH_ENGINE_FORUM_INEX_EFFICIENCY_H_

#include "search_engine_forum.h"
#include "search_engine.h"

class ANT_search_engine_forum_INEX_efficiency : public ANT_search_engine_forum
{
public:
	ANT_search_engine_forum_INEX_efficiency(char *filename, char *participant_id, char *run_id, long result_list_length, char *task);
	virtual ~ANT_search_engine_forum_INEX_efficiency();

#ifdef FILENAME_INDEX
	void write(long topic_id, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results);
#else
	void write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results);
#endif
private:
	static const char* const ID_PREFIX;
	static const long long INEX2009_index_size_bytes;
	static const long long INEX2009_indexing_time_sec;
	static const long long INEX2010_index_size_bytes;
	static const long long INEX2010_indexing_time_sec;
	static const long long INEX2010_index_size_bytes_base;
	static const long long INEX2010_indexing_time_sec_base;
} ;


#endif  /* SEARCH_ENGINE_FORUM_INEX_EFFICIENCY_H_ */
