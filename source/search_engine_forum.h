/*
	SEARCH_ENGINE_FORUM.H
	---------------------
*/
#ifndef SEARCH_ENGINE_FORUM_H_
#define SEARCH_ENGINE_FORUM_H_

#include <stdio.h>
#include "search_engine.h"
#include "focus_results_list.h"

/*
	class ANT_SEARCH_ENGINE_FORUM
	-----------------------------
*/
class ANT_search_engine_forum
{
protected:
	FILE *file;

protected:
	ANT_search_engine_forum(char *filename);

public:
	virtual ~ANT_search_engine_forum();
	virtual void write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results) = 0;
} ;

#endif  /* SEARCH_ENGINE_FORUM_H_ */

