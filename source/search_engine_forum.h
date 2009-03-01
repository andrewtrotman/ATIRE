/*
	SEARCH_ENGINE_FORUM.H
	---------------------
*/
#ifndef __SEARCH_ENGINE_FORUM_H__
#define __SEARCH_ENGINE_FORUM_H__

#include <stdio.h>

class ANT_search_engine_forum
{
protected:
	FILE *file;

protected:
	ANT_search_engine_forum(char *filename);

public:
//	ANT_search_engine_forum(char *filename, char *participant_id, char *run_id, char *task) = NULL;
	virtual ~ANT_search_engine_forum();
} ;

#endif __SEARCH_ENGINE_FORUM_H__


