/*
	SEARCH_ENGINE_FORUM.C
	---------------------
*/
#include "search_engine_forum.h"


/*
	ANT_SEARCH_ENGINE_FORUM::ANT_SEARCH_ENGINE_FORUM()
	--------------------------------------------------
*/
ANT_search_engine_forum::ANT_search_engine_forum(char *filename)
{
file = fopen(filename, "wb");
}

/*
	ANT_SEARCH_ENGINE_FORUM::~ANT_SEARCH_ENGINE_FORUM()
	---------------------------------------------------
*/
ANT_search_engine_forum::~ANT_search_engine_forum()
{
fclose(file);
}
