/*
	SEARCH_ENGINE_FORUM_INEX_BEP.C
	------------------------------
*/
#include <string.h>
#include "pragma.h"
#include "search_engine_forum_INEX_bep.h"

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_BEP::ANT_SEARCH_ENGINE_FORUM_INEX_BEP()
	--------------------------------------------------------------------
*/
ANT_search_engine_forum_INEX_bep::ANT_search_engine_forum_INEX_bep(char *filename, char *participant_id, char *run_id, char *task) : ANT_search_engine_forum(filename)
{
strncpy(this->run_id, run_id, sizeof(this->run_id));
this->run_id[sizeof(this->run_id) - 1] = '\0';

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_BEP::WRITE()
	-----------------------------------------
*/
void ANT_search_engine_forum_INEX_bep::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
{
long which;
ANT_focus_result *current;

if (focused_results != NULL)
	for (which = 0; which < hits; which++)
		if ((current = focused_results->get(which)) != NULL)
			fprintf(file, "%ld Q0 %s %lld %lld %s %lld\n", topic_id, current->document_name, (long long)(which + 1), hits - which, run_id, current->INEX_start);

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

