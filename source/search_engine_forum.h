/*
	SEARCH_ENGINE_FORUM.H
	---------------------
*/

#ifndef __SEARCH_ENGINE_FORUM_H__
#define __SEARCH_ENGINE_FORUM_H__

#include <stdio.h>

class ANT_search_engine_forum
{
private:
	FILE *file;

public:
	ANT_search_engine_forum(char *filename);
	~ANT_search_engine_forum();

	void INEX_init(void);
	void INEX_export(long topic_id, char **docids, long hits);
	void INEX_close(void);
} ;


#endif __SEARCH_ENGINE_FORUM_H__


