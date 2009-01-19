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

/*
	ANT_SEARCH_ENGINE_FORUM::INEX_INIT()
	------------------------------------
*/
void ANT_search_engine_forum::INEX_init(void)
{
fprintf(file, "<inex-submission participant-id=\"12\" run-id=\"VSM_Aggr_06\" task=\"Focused\" query=\"automatic\" result-type=\"element\">\n<topic-fields title=\"yes\" castitle=\"no\" description=\"no\" narrative=\"no\"/>\n<description>None</description>\n<collections>\n<collection>wikipedia</collection>\n</collections>\n");
}

/*
	ANT_SEARCH_ENGINE_FORUM::INEX_EXPORT()
	--------------------------------------
*/
void ANT_search_engine_forum::INEX_export(long topic_id, char **docids, long hits)
{
long list_len, which;
fprintf(file, "<topic topic-id=\"%ld\">\n", topic_id);

list_len = hits < 1500 ? hits : 1500;
for (which = 0; which < list_len; which++)
	{
	fprintf(file, "<result>\n");
	fprintf(file, "<file>%s</file>\n", docids[which]);
	fprintf(file, "<path>/article[1]</path>\n");
	fprintf(file, "<rank>%ld</rank>", which);
	fprintf(file, "</result>\n");
	}

fprintf(file, "</topic>\n");
}

/*
	ANT_SEARCH_ENGINE_FORUM::INEX_CLOSE()
	-------------------------------------
*/
void ANT_search_engine_forum::INEX_close(void)
{
fprintf(file, "</inex-submission>\n");
}
