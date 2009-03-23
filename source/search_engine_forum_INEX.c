/*
	SEARCH_ENGINE_FORUM_INEX.C
	--------------------------
*/
#include "search_engine_forum_INEX.h"

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::ANT_SEARCH_ENGINE_FORUM_INEX()
	------------------------------------------------------------
*/
ANT_search_engine_forum_INEX::ANT_search_engine_forum_INEX(char *filename, char *participant_id, char *run_id, char *task) : ANT_search_engine_forum(filename)
{
fprintf(file, "<inex-submission participant-id=\"%s\" run-id=\"%s\" task=\"%s\" query=\"automatic\" result-type=\"element\">\n", participant_id, run_id, task);
fprintf(file, "<topic-fields title=\"yes\" castitle=\"no\" description=\"no\" narrative=\"no\"/>\n");
fprintf(file, "<description>None</description>\n");
fprintf(file, "<collections>\n<collection>wikipedia</collection>\n</collections>\n");
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::~ANT_SEARCH_ENGINE_FORUM_INEX()
	-------------------------------------------------------------
*/
ANT_search_engine_forum_INEX::~ANT_search_engine_forum_INEX()
{
fprintf(file, "</inex-submission>");
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::WRITE()
	-------------------------------------
*/
void ANT_search_engine_forum_INEX::write(long topic_id, char **docids, long long hits)
{
long long which;
fprintf(file, "<topic topic-id=\"%ld\">\n", topic_id);

for (which = 0; which < hits; which++)
	{
	fprintf(file, "<result>\n");
	fprintf(file, "<file>%s</file>\n", docids[which]);
	fprintf(file, "<path>/article[1]</path>\n");
	fprintf(file, "<rank>%ld</rank>", which);
	fprintf(file, "</result>\n");
	}

fprintf(file, "</topic>\n");
}
