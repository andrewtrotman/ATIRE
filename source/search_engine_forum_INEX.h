/*
	SEARCH_ENGINE_FORUM_INEX.H
	--------------------------
*/
#ifndef __SEARCH_ENGINE_FORUM_INEX_H__
#define __SEARCH_ENGINE_FORUM_INEX_H__

#include "search_engine_forum.h"
#include "search_engine.h"

class ANT_search_engine_forum_INEX : public ANT_search_engine_forum
{
public:
	ANT_search_engine_forum_INEX(char *filename, char *participant_id, char *run_id, char *task);
	virtual ~ANT_search_engine_forum_INEX();

	void write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine);
} ;


#endif __SEARCH_ENGINE_FORUM_INEX_H__
