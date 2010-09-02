/*
	SEARCH_ENGINE_FORUM_INEX_EFFICIENCY.C
	-------------------------------------
*/
#include "pragma.h"
#include "search_engine_forum_INEX_efficiency.h"
#include "stats_search_engine.h"
#include "search_engine_accumulator.h"
#include "stdlib.h"


const char* const ANT_search_engine_forum_INEX_efficiency::ID_PREFIX = "2009-Eff-";
const long long ANT_search_engine_forum_INEX_efficiency::INEX2009_index_size_bytes = 1700305106;
const long long ANT_search_engine_forum_INEX_efficiency::INEX2009_indexing_time_sec = 4560;
const long long ANT_search_engine_forum_INEX_efficiency::INEX2010_index_size_bytes = 1962414606;
const long long ANT_search_engine_forum_INEX_efficiency::INEX2010_indexing_time_sec = 1523;
const long long ANT_search_engine_forum_INEX_efficiency::INEX2010_index_size_bytes_base = 1696382124;
const long long ANT_search_engine_forum_INEX_efficiency::INEX2010_indexing_time_sec_base = 1235;
/*
	ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY::ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY()
	----------------------------------------------------------------------------------
*/
ANT_search_engine_forum_INEX_efficiency::ANT_search_engine_forum_INEX_efficiency(char *filename, char *participant_id, char *run_id, long result_list_length, char *task) : ANT_search_engine_forum(filename)
{
fprintf(file, "<efficiency-submission ");
fprintf(file, " participant-id = \"%s\"", participant_id);
fprintf(file, " run-id = \"%s\"", run_id);
fprintf(file, " task = \"adhoc\"");
fprintf(file, " type = \"article\"");
fprintf(file, " query = \"automatic\"");
fprintf(file, " sequential = \"yes\"");
fprintf(file, " no_cpu = \"8\"");
fprintf(file, " ram = \"8GB\"");
fprintf(file, " no_nodes = \"1\"");
fprintf(file, " hardware_cost = \"3000NZD\"");
fprintf(file, " hardware_year = \"2008\"");
fprintf(file, " topk = \"%ld\"", result_list_length);
fprintf(file, " index_size_bytes = \"%lld\"", ANT_search_engine_forum_INEX_efficiency::INEX2010_index_size_bytes); //hard-coded index size, need to do manual change
fprintf(file, " indexing_time_sec = \"%lld\"", ANT_search_engine_forum_INEX_efficiency::INEX2010_indexing_time_sec); //hard-coded indexing time, need to do manual change
fprintf(file, ">\n");
fprintf(file, "<topic-fields");
fprintf(file, " co_title = \"yes\"");
fprintf(file, " cas_title = \"no\"");
fprintf(file, " xpath_title = \"no\"");
fprintf(file, " text_predicates = \"no\"");
fprintf(file, " description = \"no\"");
fprintf(file, " narrative = \"no\"");
fprintf(file, " />\n");
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY::~ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY()
	-----------------------------------------------------------------------------------
*/
ANT_search_engine_forum_INEX_efficiency::~ANT_search_engine_forum_INEX_efficiency()
{
fprintf(file, "</efficiency-submission>");
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX_EFFICIENCY::WRITE()
	------------------------------------------------
*/
void ANT_search_engine_forum_INEX_efficiency::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
{
long long which;
ANT_stats_search_engine *stats = search_engine->get_stats();
long long cpu_time_ms = stats->get_cpu_time_ms();
long long io_time_ms = stats->get_io_time_ms();

fprintf(file, "<topic");
//fprintf(file, " topic-id = \"%s%ld\"\n", ANT_search_engine_forum_INEX_efficiency::ID_PREFIX, topic_id);
fprintf(file, " topic-id = \"%ld\"\n", topic_id);
fprintf(file, " total_time_ms = \"%lld\"\n", (cpu_time_ms + io_time_ms));
fprintf(file, " cpu_time_ms = \"%lld\"\n", cpu_time_ms);
fprintf(file, " io_time_ms = \"%lld\"\n", io_time_ms);
fprintf(file, " io_bytes = \"%lld\"\n", stats->disk_bytes_read_on_search);
fprintf(file, ">\n");

for (which = 0; which < hits; which++)
	{
	fprintf(file, "<result>\n");
	fprintf(file, "<file>%s</file>\n", docids[which]);
	fprintf(file, "<path>/article[1]></path>\n");
	fprintf(file, "<rank>%lld</rank>\n", which);
	fprintf(file, "<rsv>%lld</rsv>\n", hits - which);
	fprintf(file, "</result>\n");
	}

fprintf(file, "</topic>\n");
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
