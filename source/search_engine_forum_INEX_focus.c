/*
	SEARCH_ENGINE_FORUM_INEX_FOCUS.C
	--------------------------------
*/
#include <string.h>
#include "pragma.h"
#include "search_engine_forum_INEX_focus.h"

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_FOCUS::ANT_SEARCH_ENGINE_FORUM_INEX_FOCUS()
	------------------------------------------------------------------------
*/
ANT_search_engine_forum_INEX_focus::ANT_search_engine_forum_INEX_focus(char *filename, char *participant_id, char *run_id, char *task) : ANT_search_engine_forum(filename)
{
strncpy(this->run_id, run_id, sizeof(this->run_id));
this->run_id[sizeof(this->run_id) - 1] = '\0';

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_FOCUS::WRITE()
	-------------------------------------------
*/
void ANT_search_engine_forum_INEX_focus::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
{
long which;
ANT_focus_result *current;

if (focused_results != NULL)
	for (which = 0; which < hits; which++)
		if ((current = focused_results->get(which)) != NULL)
			fprintf(file, "%ld Q0 %s %lld %lld %s %lld %lld\n", topic_id, current->document_name, (long long)(which + 1), hits - which, run_id, current->INEX_start, current->INEX_finish);

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

