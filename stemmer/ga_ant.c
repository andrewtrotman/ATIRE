/*
	GA_ANT.C
	--------
*/
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "memory.h"
#include "ctypes.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "mean_average_precision.h"
#include "disk.h"
#include "relevant_document.h"
#include "stats_time.h"
#include "stemmer.h"
#include "stemmer_factory.h"
#include "assessment_factory.h"
#include "search_engine_forum_INEX.h"
#include "search_engine_forum_TREC.h"
#include "ant_param_block.h"
#include "version.h"
#include "ga_ant.h"
#include "ranking_function.h"
#include "ranking_function_bm25.h"


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
	PERFORM_QUERY()
	---------------
*/
double perform_query(ANT_ANT_param_block *params, ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, char *query, long long *matching_documents, long topic_id, ANT_mean_average_precision *map, ANT_stemmer *stemmer = NULL)
{
ANT_stats_time stats;
long long now;
long did_query;
char token[1024];
char *token_start, *token_end;
long long hits;
size_t token_length;
ANT_search_engine_accumulator **ranked_list;
double average_precision = 0.0;

/*
	if we're stemming then create the stemmer object
*/
/*
if (!stemmer)
    stemmer = params->stemmer == 0 ? NULL : ANT_stemmer_factory::get_stemmer(params->stemmer, search_engine);
*/
search_engine->stats_initialise();		// if we are command-line then report query by query stats

did_query = FALSE;
now = stats.start_timer();
search_engine->init_accumulators();

token_end = query;

while (*token_end != '\0')
	{
	token_start = token_end;
 	while (!ANT_isalnum(*token_start) && *token_start != '\0')
		token_start++;
	if (*token_start == '\0')
		break;
	token_end = token_start;
	while (ANT_isalnum(*token_end) || *token_end == '+')
		token_end++;
	strncpy(token, token_start, token_end - token_start);
	token[token_end - token_start] = '\0';
	token_length = token_end - token_start;
//	strlwr(token);

	/*
		process the next search term - either stemmed or not.
	*/
	if (stemmer == NULL)
		search_engine->process_one_search_term(token, ranking_function);
	else
		search_engine->process_one_stemmed_search_term(stemmer, token, ranking_function);

	did_query = TRUE;
	}

/*
	Rank the results list
*/
ranked_list = search_engine->sort_results_list(params->sort_top_k, &hits); // rank

/*
	Reporting
*/

if (params->stats & ANT_ANT_param_block::SHORT)
	{
	if (topic_id >= 0)
		printf("Topic:%ld ", topic_id);
	printf("Query '%s' found %lld documents ", query, hits);
	stats.print_time("(", stats.stop_timer(now), ")");
	}

if (did_query && params->stats & ANT_ANT_param_block::QUERY)
	search_engine->stats_text_render();

/*
	Compute average previsions
*/
if (map != NULL)
    {
	if (params->metric == ANT_ANT_param_block::MAP)
		average_precision = map->average_precision(topic_id, search_engine);
	else if (params->metric == ANT_ANT_param_block::MAgP)
		average_precision = map->average_generalised_precision_document(topic_id, search_engine);
	else if (params->metric == ANT_ANT_param_block::RANKEFF)
		average_precision = map->rank_effectiveness(topic_id, search_engine);
    }

/*
	Return the number of document that matched the user's query
*/
*matching_documents = hits;

/*
	Clean up
*/
//delete stemmer;

/*
	Add the time it took to search to the global stats for the search engine
*/
search_engine->stats_add();
/*
	Return the precision
*/
return average_precision;
}

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
	ANT()
	-----
*/
#ifdef C_PLUS_PLUS_HAS_NESTABLE_COMMENTS
double ant(ANT_search_engine *search_engine, ANT_mean_average_precision *map, ANT_ANT_param_block *params, char **document_list, char **answer_list)
{
char *query;
long topic_id, line;
long long hits, result, last_to_list;
double average_precision, sum_of_average_precisions, mean_average_precision;
ANT_ANT_file_iterator input(params->queries_filename);
ANT_search_engine_forum *output = NULL;
long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;

if (params->output_forum == ANT_ANT_param_block::TREC)
	output = new ANT_search_engine_forum_TREC(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == ANT_ANT_param_block::INEX)
	output = new ANT_search_engine_forum_INEX(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");

sum_of_average_precisions = 0.0;
line = 0;
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

	if (!have_assessments)
		topic_id = -1;
	else
		{
		topic_id = atol(query);
		if ((query = strchr(query, ' ')) == NULL)
			exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, query));
		}

	/*
		Do the query and compute average precision
	*/
	average_precision = perform_query(params, search_engine, query, &hits, topic_id, map);
	sum_of_average_precisions += average_precision;

	/*
		Report the average precision for the query
	*/
	if (map != NULL && params->stats & ANT_ANT_param_block::SHORT)
		printf("Topic:%ld Average Precision:%f\n", topic_id , average_precision);

	/*
		Convert from a results list into a list of documents
	*/
	search_engine->generate_results_list(document_list, answer_list, hits);

	/*
		Display the list of results (either to the user or to a run file)
	*/
	last_to_list = hits > params->results_list_length ? params->results_list_length : hits;
	if (output == NULL)
		for (result = 0; result < last_to_list; result++)
			printf("%lld:%s\n", result + 1, answer_list[result]);
	else
		output->write(topic_id, answer_list, last_to_list);
	prompt(params);
	}
/*
	Compute Mean Average Precision
*/
mean_average_precision = sum_of_average_precisions / (double) (line - 1);

/*
	Report MAP
*/
if (map != NULL && params->stats & ANT_ANT_param_block::SHORT)
	printf("\nProcessed %ld topics (MAP:%f)\n\n", line - 1, mean_average_precision);

/*
	Report the summary of the stats
*/
if (params->stats & ANT_ANT_param_block::SUM)
	search_engine->stats_all_text_render();

/*
	And finally report MAP
*/
return mean_average_precision;
}

#endif 
/*
	READ_DOCID_LIST()
	-----------------
*/
char **read_docid_list(long long *documents_in_id_list, char ***filename_list)
{
char *document_list_buffer, *filename_list_buffer;

if ((document_list_buffer = ANT_disk::read_entire_file("doclist.aspt")) == NULL)
	exit(printf("Cannot open document ID list file 'doclist.aspt'\n"));

filename_list_buffer = strnew(document_list_buffer);
*filename_list = ANT_disk::buffer_to_list(filename_list_buffer, documents_in_id_list);

return ANT_disk::buffer_to_list(document_list_buffer, documents_in_id_list);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_search_engine *search_engine;
//thesaurus_engine *search_engine;
ANT_mean_average_precision *map = NULL;
ANT_memory memory;
long last_param;
 char **document_list, **answer_list, **filename_list;
ANT_relevant_document *assessments = NULL;
long long documents_in_id_list, number_of_assessments;

#ifndef VOCAB_TOOL
char *stemmer_file = NULL;
if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 's') {
     stemmer_file = argv[1] + 2;
     argc--;
     argv++;
 }
#endif

ANT_ANT_param_block params(argc, argv);

last_param = params.parse();

if (params.logo)
	puts(ANT_version_string);				// print the version string if we parsed the parameters OK

 document_list = read_docid_list(&documents_in_id_list, &filename_list);

if (params.assessments_filename != NULL)
	{
	ANT_assessment_factory factory(&memory, document_list, documents_in_id_list);
	assessments = factory.read(params.assessments_filename, &number_of_assessments);
	map = new ANT_mean_average_precision(&memory, assessments, number_of_assessments);
	}

answer_list = (char **)memory.malloc(sizeof(*answer_list) * documents_in_id_list);

search_engine = new ANT_search_engine(&memory);
//search_engine = new thesaurus_engine(&memory, 0.0);
//search_engine->stemming_exceptions(ANT_stemmer_factory::get_stemmer(ANT_stemmer_factory::PORTER, search_engine), 0.8);

#ifdef VOCAB_TOOL
trie_test(search_engine);
#else
ga_ant(search_engine, map, &params, document_list, filename_list, answer_list, stemmer_file);
#endif

delete map;
return 0;
}


/* 
*** MY ADDITION ***

   GET_QUERIES()
   fill an array with queries from params->queries_filename
*/
char **get_queries(long *query_count, ANT_ANT_param_block *params) {
    ANT_ANT_file_iterator input(params->queries_filename);
	char **queries;
    char *query;
    int i;
    *query_count = 0;
    for (query = input.first(); query != NULL; query = input.next()) {
        strip_space_inplace(query);
        if (query != '\0')
            (*query_count)++;
    }
    queries = (char **)malloc(sizeof *queries * *query_count);
    for (query = input.first(), i = 0; query != NULL; query = input.next(), i++) {
        strip_space_inplace(query);
        if (query == '\0')
            continue;

        queries[i] = strdup(query);
    }
    return queries;
}
