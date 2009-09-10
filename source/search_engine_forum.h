/*
	SEARCH_ENGINE_FORUM.H
	---------------------
*/
#ifndef __SEARCH_ENGINE_FORUM_H__
#define __SEARCH_ENGINE_FORUM_H__

#include <stdio.h>
#include "search_engine.h"

class ANT_search_engine_forum
{
protected:
	FILE *file;

protected:
	ANT_search_engine_forum(char *filename);

public:
	virtual ~ANT_search_engine_forum();
	virtual void write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine) = 0;
} ;

#endif __SEARCH_ENGINE_FORUM_H__


