/*
 * ANT_ANT.C
 * ---------
 *
 */

/*
	ANT.C
	-----
*/
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "memory.h"
#include "ctypes.h"
#include "search_engine.h"
#include "search_engine_readability.h"
#include "search_engine_btree_leaf.h"
#include "mean_average_precision.h"
#include "disk.h"
#include "relevant_document.h"
#include "time_stats.h"
#include "stemmer.h"
#include "stemmer_factory.h"
#include "assessment_factory.h"
#include "search_engine_forum_INEX.h"
#include "search_engine_forum_TREC.h"
#include "ant_param_block.h"
#include "encoding_utf8.h"
#include "version.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_similarity.h"
#include "ranking_function_lmd.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"

#include "ant_api.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char *PROMPT = "]";

/*
	class ANT_ANT_FILE_ITERATOR
	---------------------------
*/
class ANT_ANT_file_iterator
{
private:
	FILE *fp;
	char query[1024];

public:
	ANT_ANT_file_iterator(char *filename)
		{
		if (filename == NULL)
			fp = stdin;
		else
			fp = fopen(filename, "rb");
		if (fp == NULL)
			exit(printf("Cannot open topic file:'%s'\n", filename));
		}
	~ANT_ANT_file_iterator() { if (fp != NULL) fclose(fp); }
	char *first(void) {
        fseek(fp, 0, SEEK_SET);
        return fgets(query, sizeof(query), fp);
    }
	char *next(void) { return fgets(query, sizeof(query), fp); }
} ;

/*
	PROMPT()
	--------
*/
void prompt(ANT_ANT_param_block *params)
{
if (params->queries_filename == NULL)
	printf(PROMPT);
}

/*
	GET_DOCUMENT_AND_PARSE()
	------------------------
*/
char *get_document_and_parse(char *filename, ANT_time_stats *stats)
{
static char filename_buffer[1024];
char *start, *end, *file;
long long now, length;

if (filename[1] == ':')							// windows c:\blah
	filename += 2;

now = stats->start_timer();
file = ANT_disk::read_entire_file(filename);
stats->add_disk_input_time(stats->stop_timer(now));

if (file == NULL)
	return NULL;

now = stats->start_timer();
start = strstr(file, "<title>");
if (start != NULL)
	if ((end = strstr(start += 7, "</title>")) != NULL)
		{
		length = end - start < 1022 ? end - start : 1022;
		strncpy(filename_buffer, start, length);
		filename_buffer[length] = '\0';
		}

delete [] file;

stats->add_cpu_time(stats->stop_timer(now));

return *filename_buffer == '\0' ? NULL : filename_buffer;
}

/*
	ANT()
	-----
*/
double ant(ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, ANT_mean_average_precision *map, ANT_ANT_param_block *params, char **filename_list, char **document_list, char **answer_list)
{
ANT_time_stats post_processing_stats;
char *query, *name;
long topic_id, line, number_of_queries;
long long hits, result, last_to_list;
double average_precision, sum_of_average_precisions, mean_average_precision;
ANT_ANT_file_iterator input(params->queries_filename);
ANT_search_engine_forum *output = NULL;
long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;
ANT_stemmer *stemmer = params->stemmer == 0 ? NULL : ANT_stemmer_factory::get_stemmer(params->stemmer, search_engine, params->stemmer_similarity, params->stemmer_similarity_threshold);

if (params->output_forum == ANT_ANT_param_block::TREC)
	output = new ANT_search_engine_forum_TREC(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == ANT_ANT_param_block::INEX)
	output = new ANT_search_engine_forum_INEX(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");

sum_of_average_precisions = 0.0;
number_of_queries = line = 0;
prompt(params);
for (query = input.first(); query != NULL; query = input.next())
	{
	line++;
	/*
		Parsing to get the topic number
	*/
	strip_space_inplace(query);
	if (strcmp(query, ".quit") == 0)
		break;
	if (*query == '\0')
		continue;			// ignore blank lines

	if (have_assessments || params->output_forum != ANT_ANT_param_block::NONE || params->queries_filename != NULL)
		{
		topic_id = atol(query);
		if ((query = strchr(query, ' ')) == NULL)
			exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, query));
		}
	else
		topic_id = -1;

	/*
		Do the query and compute average precision
	*/
	number_of_queries++;

	average_precision = perform_query(params, search_engine, ranking_function, query, &hits, topic_id, map, stemmer);
	sum_of_average_precisions += average_precision;

	/*
		Report the average precision for the query
	*/
	if (map != NULL && params->stats & ANT_ANT_param_block::SHORT)
		printf("Topic:%ld Average Precision:%f\n", topic_id , average_precision);

	/*
		Convert from a results list into a list of documents
	*/
	if (output == NULL)
		search_engine->generate_results_list(filename_list, answer_list, hits);
	else
		search_engine->generate_results_list(document_list, answer_list, hits);

	/*
		Display the list of results (either to the user or to a run file)
	*/
	last_to_list = hits > params->results_list_length ? params->results_list_length : hits;
	if (output == NULL)
		for (result = 0; result < last_to_list; result++)
			if ((name = get_document_and_parse(answer_list[result], &post_processing_stats)) == NULL)
				printf("%lld:%s\n", result + 1, answer_list[result]);
			else
				printf("%lld:(%s) %s\n", result + 1, answer_list[result], name);
	else
		output->write(topic_id, answer_list, last_to_list);

	prompt(params);
	}

/*
	Compute Mean Average Precision
*/
mean_average_precision = sum_of_average_precisions / (double)number_of_queries;

/*
	Report MAP
*/
if (map != NULL && params->stats & ANT_ANT_param_block::SHORT)
	printf("\nProcessed %ld topics (MAP:%f)\n\n", number_of_queries, mean_average_precision);

/*
	Report the summary of the stats
*/
if (params->stats & ANT_ANT_param_block::SUM)
	{
	search_engine->stats_all_text_render();
	post_processing_stats.print_time("Post Processing I/O  :", post_processing_stats.disk_input_time);
	post_processing_stats.print_time("Post Processing CPU  :", post_processing_stats.cpu_time);
	}

/*
	And finally report MAP
*/
return mean_average_precision;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_ANT_param_block params(argc, argv);
long last_param;

last_param = params.parse();

if (params.logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

document_list = read_docid_list(&documents_in_id_list, &filename_list, &mem1, &mem2);

if (params.assessments_filename != NULL)
	{
	ANT_assessment_factory factory(&memory, document_list, documents_in_id_list);
	assessments = factory.read(params.assessments_filename, &number_of_assessments);
	map = new ANT_mean_average_precision(&memory, assessments, number_of_assessments);
	}

answer_list = (char **)memory.malloc(sizeof(*answer_list) * documents_in_id_list);

if (params.ranking_function == ANT_ANT_param_block::READABLE)
	{
	search_engine = readable_search_engine = new ANT_search_engine_readability(&memory);
	ranking_function = new ANT_ranking_function_readability(readable_search_engine);
	}
else
	{
	search_engine = new ANT_search_engine(&memory);
    /*    	if (params.stemmer_similarity == ANT_ANT_param_block::WEIGHTED)
		ranking_function = new ANT_ranking_function_similarity(search_engine, params.bm25_k1, params.bm25_b);
        else*/
    if (params.ranking_function == ANT_ANT_param_block::BM25)
		ranking_function = new ANT_ranking_function_BM25(search_engine, params.bm25_k1, params.bm25_b);
	else if (params.ranking_function == ANT_ANT_param_block::IMPACT)
		ranking_function = new ANT_ranking_function_impact(search_engine);
	else if (params.ranking_function == ANT_ANT_param_block::LMD)
		ranking_function = new ANT_ranking_function_lmd(search_engine, params.lmd_u);
	else if (params.ranking_function == ANT_ANT_param_block::LMJM)
		ranking_function = new ANT_ranking_function_lmjm(search_engine, params.lmjm_l);
	else if (params.ranking_function == ANT_ANT_param_block::BOSE_EINSTEIN)
        ranking_function = new ANT_ranking_function_bose_einstein(search_engine);
	else if (params.ranking_function == ANT_ANT_param_block::DIVERGENCE)
		ranking_function = new ANT_ranking_function_divergence(search_engine);
	}
//printf("Index contains %lld documents\n", search_engine->document_count());

search_engine->set_trim_postings_k(params.trim_postings_k);
ant(search_engine, ranking_function, map, &params, filename_list, document_list, answer_list);



printf("Total elapsed time including startup and shutdown ");
stats.print_elapsed_time();
ANT_stats::print_operating_system_process_time();
return 0;
}
