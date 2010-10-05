/*
	SEARCH_ENGINE_FORUM_TREC.C
	--------------------------
*/
#include <string.h>
#include "pragma.h"
#include "search_engine_forum_TREC.h"

/*
	ANT_SEARCH_ENGINE_FORUM_TREC::ANT_SEARCH_ENGINE_FORUM_TREC()
	------------------------------------------------------------
*/
ANT_search_engine_forum_TREC::ANT_search_engine_forum_TREC(char *filename, char *participant_id, char *run_id, char *task) : ANT_search_engine_forum(filename)
{
strncpy(this->run_id, run_id, sizeof(this->run_id));
this->run_id[sizeof(this->run_id) - 1] = '\0';
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_SEARCH_ENGINE_FORUM_TREC::WRITE()
	-------------------------------------
*/
void ANT_search_engine_forum_TREC::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
{
long long which;
ANT_search_engine_accumulator **accumulator_pointers = search_engine->results_list->accumulator_pointers;

for (which = 0; which < hits; which++)
	fprintf(file, "%ld Q0 %s %lld %f %s\n", topic_id, docids[which], which + 1, (double)accumulator_pointers[which]->get_rsv(), run_id);
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
