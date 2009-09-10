/*
	SEARCH_ENGINE_FORUM_INEX_EFFICIENCY.C
	--------------------------
*/
#include "search_engine_forum_INEX_efficiency.h"
#include "search_engine_stats.h"
#include "search_engine_accumulator.h"
#include "stdlib.h"


const char* const ANT_search_engine_forum_INEX_efficiency::ID_PREFIX = "2009-Eff-";

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY::ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY()
	------------------------------------------------------------
*/
ANT_search_engine_forum_INEX_efficiency::ANT_search_engine_forum_INEX_efficiency(char *filename, char *participant_id, char *run_id, long result_list_length, char *task) : ANT_search_engine_forum(filename)
{
	printf("\n\nUsing INEX efficiency\n\n");
	fprintf(file, "<!ELEMENT efficiency-submission (topic-fields,\n");
	fprintf(file, "                            general_description,\n");
	fprintf(file, "                            ranking_description,\n");
	fprintf(file, "                            indexing_description,\n");
	fprintf(file, "                            caching_description,\n");
	fprintf(file, "                            topic+)>\n");
	fprintf(file, "<!ATTLIST efficiency-submission\n");
	fprintf(file, "  participant-id %s\n", participant_id);
	fprintf(file, "  run-id         %s\n", run_id);
	fprintf(file, "  task           adhoc\n");
	fprintf(file, "  type           article\n");
	fprintf(file, "  query          automatic\n");
	fprintf(file, "  sequential     yes\n");
	fprintf(file, "  no_cpu         8\n");
	fprintf(file, "  ram            8GB\n");
	fprintf(file, "  no_nodes       1\n");
	fprintf(file, "  hardware_cost  3000NZD\n");
	fprintf(file, "  hardware_year  2008\n");
	fprintf(file, "  topk           %ld\n", result_list_length);
	fprintf(file, ">\n");
	fprintf(file, "<!ELEMENT topic-fields EMPTY>\n");
	fprintf(file, "<!ATTLIST topic-fields\n");
	fprintf(file, "  co_title        yes\n");
	fprintf(file, "  cas_title       no\n");
	fprintf(file, "  xpath_title     no\n");
	fprintf(file, "  text_predicates no\n");
	fprintf(file, "  description     no\n");
	fprintf(file, "  narrative       no\n");
	fprintf(file, ">\n");
	fprintf(file, "<!ELEMENT topic (result*)>\n");
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY::~ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY()
	-------------------------------------------------------------
*/
ANT_search_engine_forum_INEX_efficiency::~ANT_search_engine_forum_INEX_efficiency()
{

}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY::WRITE()
	-------------------------------------
*/
void ANT_search_engine_forum_INEX_efficiency::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine)
{
	long long which;
	ANT_search_engine_stats *stats = search_engine->get_stats();
	long long cpu_time_ms = stats->get_cpu_time_ms();
	long long io_time_ms = stats->get_io_time_ms();
	ANT_search_engine_accumulator **accumulator_pointers = search_engine->get_accumulator_pointers();

	fprintf(file, "<!ATTLIST topic\n");
	fprintf(file, "  topic-id       %s%ld>\n", ANT_search_engine_forum_INEX_efficiency::ID_PREFIX, topic_id);
	fprintf(file, "  total_time_ms  %lld\n", (cpu_time_ms + io_time_ms));
	fprintf(file, "  cpu_time_ms    %lld\n", cpu_time_ms);
	fprintf(file, "  io_time_ms     %lld\n", io_time_ms);
	fprintf(file, "  io_bytes       %lld\n", stats->disk_bytes_read_on_search);
	fprintf(file, ">");

	for (which = 0; which < hits; which++) {
		fprintf(file, "<!ELEMENT result (file, path, rank, rsv?)>\n");
		fprintf(file, "<!ELEMENT file   %s>\n", docids[which]);
		fprintf(file, "<!ELEMENT path   /article[1]>\n");
		fprintf(file, "<!ELEMENT rank   %lld>\n", which);
		fprintf(file, "<!ELEMENT rsv    %.3f>\n", accumulator_pointers[which]->get_rsv());
	}

}
