/*
	SEARCH_ENGINE_FORUM_INEX.H
	--------------------------
*/
#ifndef SEARCH_ENGINE_FORUM_INEX_H_
#define SEARCH_ENGINE_FORUM_INEX_H_

#include "search_engine_forum.h"
#include "search_engine.h"

class ANT_focus_result;

/*
	class ANT_SEARCH_ENGINE_FORUM_INEX
	----------------------------------
*/
class ANT_search_engine_forum_INEX : public ANT_search_engine_forum
{
public:
	ANT_search_engine_forum_INEX(char *filename, char *participant_id, char *run_id, char *task);
	virtual ~ANT_search_engine_forum_INEX();

	static ANT_focus_result *focus_to_INEX(char *document, ANT_focus_result *result);

	void write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results);
} ;


#endif  /* SEARCH_ENGINE_FORUM_INEX_H_ */
