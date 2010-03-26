/*
	SEARCH_ENGINE_FORUM_INEX_FOCUS.H
	--------------------------------
*/
#ifndef SEARCH_ENGINE_FORUM_INEX_FOCUS_H_
#define SEARCH_ENGINE_FORUM_INEX_FOCUS_H_

#include "search_engine_forum.h"
#include "search_engine.h"

#define MAX_RUN_NAME_LENGTH 1024

/*
	class ANT_SEARCH_ENGINE_FORUM_INEX_FOCUS
	----------------------------------------
*/
class ANT_search_engine_forum_INEX_focus : public ANT_search_engine_forum
{
private:
	char run_id[MAX_RUN_NAME_LENGTH];

public:
	ANT_search_engine_forum_INEX_focus(char *filename, char *participant_id, char *run_id, char *task);
	void write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results);
} ;

#endif /* SEARCH_ENGINE_FORUM_INEX_FOCUS_H_ */

