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
#ifdef FILENAME_INDEX
	void ANT_search_engine_forum_TREC::write(long topic_id, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
	{
	long long result, docid;
	double relevance;
	char filename[128];

	for (result = 0; result < hits; result++)
		{
		relevance = search_engine->results_list->accumulator_pointers[result]->get_rsv();
		docid = search_engine->results_list->accumulator_pointers[result] - search_engine->results_list->accumulator;
		search_engine->get_document_filename(filename, docid);

		fprintf(file, "%ld Q0 %s %lld %f %s\n", topic_id, filename, result + 1, relevance, run_id);
		}
	#pragma ANT_PRAGMA_UNUSED_PARAMETER
	}
#else
	void ANT_search_engine_forum_TREC::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
	{
	long long which;
	ANT_search_engine_accumulator **accumulator_pointers = search_engine->results_list->accumulator_pointers;

	for (which = 0; which < hits; which++)
		fprintf(file, "%ld Q0 %s %lld %f %s\n", topic_id, docids[which], which + 1, (double)accumulator_pointers[which]->get_rsv(), run_id);
	#pragma ANT_PRAGMA_UNUSED_PARAMETER
	}
#endif
