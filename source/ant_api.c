/*
 * ANT_API.C
 *
 */

#include "ant_api.h"

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
#include "search_engine_forum_INEX_efficiency.h"
#include "search_engine_forum_TREC.h"
#include "ant_params.h"
#include "version.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_similarity.h"
#include "ranking_function_lmd.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"
#include "parser.h"
#include "NEXI.h"
#include "NEXI_term_iterator.h"

#include <limits.h>
#include <stdlib.h>

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
 * ANT_ANT_HANDLE
 * --------------
 */
struct ANT_ant_handle
{
ANT_stats stats;
ANT_time_stats *post_processing_stats;
ANT_ANT_params params;
ANT_search_engine *search_engine;
ANT_search_engine_readability *readable_search_engine;
ANT_mean_average_precision *map;
ANT_memory memory;
char **document_list, **answer_list, **filename_list;
ANT_relevant_document *assessments;
long long documents_in_id_list, number_of_assessments;
ANT_ranking_function *ranking_function;
char *mem1, *mem2;

double average_precision, sum_of_average_precisions;
long number_of_queries;
};

/*
	ANT_PARAMS()
	-----------
*/
ANT_ANT_params *ant_params(ANT *ant)
{
struct ANT_ant_handle *data = (ANT_ant_handle *)ant;
return &data->params;
}

/*
	ANT_PARAMS_INIT()
	----------
*/
void ant_params_init(ANT *ant)
{
struct ANT_ANT_params *params = ant_params(ant);

params->logo = TRUE;
params->stemmer = 0;
params->stemmer_similarity = FALSE;
params->stemmer_similarity_threshold = 0.0;
params->sort_top_k = LLONG_MAX;
params->metric = MAP;
params->assessments_filename = NULL;
params->queries_filename = NULL;
params->output_forum = NONE;
params->run_name = params->participant_id = "unknown";
params->output_filename = "ant.out";
params->results_list_length = -1;
params->stats = SHORT;
params->segmentation = FALSE;
params->ranking_function = BM25;
params->trim_postings_k = LLONG_MAX;
params->lmd_u = 500.0;
params->lmjm_l = 0.5;
params->bm25_k1 = 0.9;
params->bm25_b = 0.4;
params->output = stdout;
params->index_filename = "index.aspt";
params->doclist_filename = "doclist.aspt";
}

/*
	ANT_POST_PROCESSING_STATS_INIT()
	---------------
*/
void ant_post_processing_stats_init(ANT *ant)
{
((ANT_ant_handle *)ant)->post_processing_stats = new ANT_time_stats;
}

/*
	ANT_EASY_INIT()
	---------------
*/
ANT *ant_easy_init()
{
struct ANT_ant_handle *data = (struct ANT_ant_handle *)calloc(1, sizeof(struct ANT_ant_handle));

data->map = NULL;
data->assessments = NULL;
data->ranking_function = NULL;
data->average_precision = 0.0;
data->sum_of_average_precisions = 0.0;
data->number_of_queries = 0;
data->post_processing_stats = NULL;

ant_params_init((ANT *)data);

return data;
}

/*
	ANT_FILENAMES_INIT()
	--------------------
*/
void ant_filenames_init(ANT* ant, const char *doclist_filename, const char *index_filename)
{
struct ANT_ANT_params *params = ant_params(ant);
params->doclist_filename = doclist_filename;
params->index_filename = index_filename;
}

/*
	MAX()
	-----
*/
char *max(char *a, char *b, char *c)
{
char *thus_far;

thus_far = a;
if (b > thus_far)
	thus_far = b;
if (c > thus_far)
	thus_far = c;

return thus_far;
}

/*
	READ_DOCID_LIST()
	-----------------
*/
char **read_docid_list(const char *doclist_filename, long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2)
{
char *document_list_buffer, *filename_list_buffer;
char **id_list, **current;
char *slish, *slash, *slosh, *start, *dot;

if ((document_list_buffer = ANT_disk::read_entire_file((char *)doclist_filename)) == NULL)
	exit(fprintf(stderr, "Cannot open document ID list file 'doclist.aspt'\n"));

filename_list_buffer = strnew(document_list_buffer);
*filename_list = ANT_disk::buffer_to_list(filename_list_buffer, documents_in_id_list);

id_list = ANT_disk::buffer_to_list(document_list_buffer, documents_in_id_list);

/*
	This code converts filenames into DOCIDs
*/
for (current = id_list; *current != NULL; current++)
	{
	slish = *current;
	slash = strrchr(*current, '/');
	slosh = strrchr(*current, '\\');
	start = max(slish, slash, slosh);		// get the posn of the final dir seperator (or the start of the string)
	if (*start != '\0')		// avoid blank lines at the end of the file
		{
		if ((dot = strchr(start, '.')) != NULL)
			*dot = '\0';
		*current = start == *current ? *current : start + 1;		// +1 to skip over the '/'
		}
	}
/*
	The caller must delete these two on termination
*/
*mem1 = document_list_buffer;
*mem2 = filename_list_buffer;

/*
	Now return the list
*/
return id_list;
}

/*
	ANT_SETUP()
	----------
*/
void ant_setup(ANT *ant)
{
struct ANT_ANT_params *params = ant_params(ant);
struct ANT_ant_handle *data = (ANT_ant_handle *)ant;

if (params->logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

data->document_list = read_docid_list(params->doclist_filename, &data->documents_in_id_list, &data->filename_list, &data->mem1, &data->mem2);

if (params->assessments_filename != NULL)
	{
	ANT_assessment_factory factory(&data->memory, data->document_list, data->documents_in_id_list);
	data->assessments = factory.read(params->assessments_filename, &data->number_of_assessments);
	data->map = new ANT_mean_average_precision(&data->memory, data->assessments, data->number_of_assessments);
	}

data->answer_list = (char **)data->memory.malloc(sizeof(*data->answer_list) * data->documents_in_id_list);

if (params->ranking_function == READABLE)
	{
	data->search_engine = data->readable_search_engine = new ANT_search_engine_readability(params->index_filename, &data->memory);
	data->ranking_function = new ANT_ranking_function_readability(data->readable_search_engine);
	}
else
	{
	data->search_engine = new ANT_search_engine(params->index_filename, &data->memory);
    /*    	if (params->stemmer_similarity == ANT_ANT_param_block::WEIGHTED)
		ranking_function = new ANT_ranking_function_similarity(search_engine, params->bm25_k1, params->bm25_b);
        else*/
    if (params->ranking_function == BM25)
	    data->ranking_function = new ANT_ranking_function_BM25(data->search_engine, params->bm25_k1, params->bm25_b);
	else if (params->ranking_function == IMPACT)
	    data->ranking_function = new ANT_ranking_function_impact(data->search_engine);
	else if (params->ranking_function == LMD)
	    data->ranking_function = new ANT_ranking_function_lmd(data->search_engine, params->lmd_u);
	else if (params->ranking_function == LMJM)
	    data->ranking_function = new ANT_ranking_function_lmjm(data->search_engine, params->lmjm_l);
	else if (params->ranking_function == BOSE_EINSTEIN)
	    data->ranking_function = new ANT_ranking_function_bose_einstein(data->search_engine);
	else if (params->ranking_function == DIVERGENCE)
	    data->ranking_function = new ANT_ranking_function_divergence(data->search_engine);
	}
//printf("Index contains %lld documents\n", search_engine->document_count());

data->search_engine->set_trim_postings_k(params->trim_postings_k);
}

/*
	PERFORM_QUERY()
	---------------
*/
double perform_query(ANT_ANT_params *params, ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, char *query, long long *matching_documents, long topic_id, ANT_mean_average_precision *map, ANT_stemmer *stemmer = NULL)
{
ANT_time_stats stats;
long long now, hits;
long did_query, first_case, token_length;
char *current, token[1024];
ANT_search_engine_accumulator *ranked_list;
double average_precision = 0.0;
ANT_NEXI parser;
ANT_NEXI_term_iterator term;
ANT_NEXI_term *term_string;

search_engine->stats_initialise();		// if we are command-line then report query by query stats

did_query = FALSE;
now = stats.start_timer();
search_engine->init_accumulators();

for (term_string = term.first(parser.parse(query)); term_string != NULL; term_string = term.next())
	{
	/*
		Take the search term (as an ANT_string_pair) and convert into a string
		If you want to know if the term is a + or - term then call term_string->get_sign() which will return 0 if it is not (or +ve or -ve if it is)
	*/
	token_length = term_string->get_term()->string_length < sizeof(token) - 1 ? term_string->get_term()->string_length : sizeof(token) - 1;
	strncpy(token, term_string->get_term()->start, token_length);
	token[token_length] = '\0';

	/*
		Terms that are in upper-case are tag names for the bag-of-tags approach whereas mixed / lower case terms are search terms
		but as the vocab is in lower case it is necessary to check then convert.
	*/
	first_case = ANT_islower(*token);
	for (current = token; *current != '\0'; current++)
		if (ANT_islower(*current) != first_case)
			{
			strlower(token);
			break;
			}

	/*
		process the next search term - either stemmed or not.
	*/
	if (stemmer == NULL || !ANT_islower(*token))		// so we don't stem numbers of tag names
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
if (params->stats & SHORT)
	{
	if (topic_id >= 0)
		fprintf(params->output, "Topic:%ld ", topic_id);
	fprintf(params->output, "Query '%s' found %lld documents ", query, hits);
	stats.print_time("(", stats.stop_timer(now), ")");
	}

if (did_query && params->stats & QUERY)
	search_engine->stats_text_render();

/*
	Compute average previsions
*/
if (map != NULL)
	{
	if (params->metric == MAP)
		average_precision = map->average_precision(topic_id, search_engine);
	else if (params->metric == MAgP)
		average_precision = map->average_generalised_precision(topic_id, search_engine);
	else if (params->metric == RANKEFF)
		average_precision = map->rank_effectiveness(topic_id, search_engine);
	}

/*
	Return the number of document that matched the user's query
*/
*matching_documents = hits;

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
double ant_perform(ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, ANT_mean_average_precision *map, ANT_ANT_params *params, char *query, char **filename_list, char **document_list, char **answer_list, long long *num_of_retrieved, long topic_id)
{
char /**query, */*name;
//long topic_id, line, number_of_queries;
long long hits, result, last_to_list;
double average_precision = 0.0;

//ANT_ANT_file_iterator input(params->queries_filename);
//long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;
ANT_stemmer *stemmer = params->stemmer == 0 ? NULL : ANT_stemmer_factory::get_stemmer(params->stemmer, search_engine, params->stemmer_similarity, params->stemmer_similarity_threshold);

//sum_of_average_precisions = 0.0;
//number_of_queries = line = 0;
//prompt(params);
//for (query = input.first(); query != NULL; query = input.next())
	//{
	//line++;
	/*
		Parsing to get the topic number
	*/
	strip_space_inplace(query);
//	if (strcmp(query, ".quit") == 0)
//		break;
//	if (*query == '\0')
//		continue;			// ignore blank lines

//	if (have_assessments || params->output_forum != NONE || params->queries_filename != NULL)
//		{
//		topic_id = atol(query);
//		if ((query = strchr(query, ' ')) == NULL)
//			exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, query));
//		}
//	else
//		topic_id = -1;

	/*
		Do the query and compute average precision
	*/
	//number_of_queries++;

	average_precision = perform_query(params, search_engine, ranking_function, query, &hits, topic_id, map, stemmer);
	//sum_of_average_precisions += average_precision;

	/*
		Report the average precision for the query
	*/
	if (map != NULL && params->stats & SHORT)
		fprintf(params->output, "Topic:%ld Average Precision:%f\n", topic_id , average_precision);

	//prompt(params);
	//}

*num_of_retrieved = hits;
return average_precision;
}

/*
	ANT_SEARCH()
	-----------
*/
char **ant_search(ANT *ant, long long *hits, char *query, long topic_id)
{
struct ANT_ant_handle *data = (ANT_ant_handle *)ant;
struct ANT_ANT_params *params = ant_params(ant);

long long result;
long long last_to_list = 0;
char *name;

data->sum_of_average_precisions += ant_perform(data->search_engine, data->ranking_function, data->map, ant_params(ant), query, data->filename_list, data->document_list, data->answer_list, hits, topic_id);
data->number_of_queries++;

ANT_search_engine_forum *output = NULL;
if (params->output_forum == TREC)
	output = new ANT_search_engine_forum_TREC(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == INEX)
	output = new ANT_search_engine_forum_INEX(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == INEX_EFFICIENCY) {
	output = new ANT_search_engine_forum_INEX_efficiency(params->output_filename, params->participant_id, params->run_name, params->results_list_length, "RelevantInContext");
}
/*
	Convert from a results list into a list of documents
*/
if (output == NULL)
    data->search_engine->generate_results_list(data->filename_list, data->answer_list, *hits);
else
    data->search_engine->generate_results_list(data->document_list, data->answer_list, *hits);


/*
	Display the list of results (either to the user or to a run file)
*/
last_to_list = (*hits) > params->results_list_length ? params->results_list_length : (*hits);
if (output == NULL)
	for (result = 0; result < last_to_list; result++)
		if ((name = get_document_and_parse(data->answer_list[result], data->post_processing_stats)) == NULL)
			fprintf(params->output, "%lld:%s\n", result + 1, data->answer_list[result]);
		else
			fprintf(params->output, "%lld:(%s) %s\n", result + 1, data->answer_list[result], name);
else
	output->write(topic_id, data->answer_list, last_to_list, data->search_engine);

/* free the allocated forum */
delete output;

return data->answer_list;
}

/*
	ANT_GET_COLLECTION_DETAILS()
	----------------------------
*/
struct collection_details_s *ant_get_collection_details(ANT *ant, struct collection_details_s *collection_details) 
{
    ANT_search_engine *search_engine = ((ANT_ant_handle *)ant)->search_engine;

    collection_details->documents_in_collection = search_engine->document_count();
    collection_details->terms_in_collection = search_engine->get_collection_length();

    return collection_details;
}

/*
	ANT_GET_TERM_DETAILS()
	----------------------
*/
struct term_details_s *ant_get_term_details(ANT *ant, char *term, struct term_details_s *term_details) 
{
    ANT_search_engine *search_engine = ((ANT_ant_handle *)ant)->search_engine;
    ANT_search_engine_btree_leaf internal_details;
    

	if (search_engine->get_postings_details(term, &internal_details))
		{
		term_details->collection_frequency = internal_details.collection_frequency;
		term_details->document_frequency = internal_details.document_frequency;
		}
 	else 
		{
		term_details->collection_frequency = 0;
		term_details->document_frequency = 0;
		}
    return term_details;
}

/*
	ANT_CAL_MAP()
	-------------
*/
double ant_cal_map(ANT *ant)
{
struct ANT_ant_handle *data = (ANT_ant_handle *)ant;
struct ANT_ANT_params *params = ant_params(ant);
/*
	Compute Mean Average Precision
*/
double mean_average_precision = data->sum_of_average_precisions / (double)data->number_of_queries;

/*
	Report MAP
*/
if (data->map != NULL && params->stats & SHORT)
	fprintf(stderr, "\nProcessed %ld topics (MAP:%f)\n\n", data->number_of_queries, mean_average_precision);

/*
	And finally report MAP
*/
return mean_average_precision;
}

/*
	ANT_STAT()
	-----------
*/
void ant_stat(ANT *ant)
{
struct ANT_ant_handle *data = (ANT_ant_handle *)ant;
struct ANT_ANT_params *params = ant_params(ant);

/*
	Report the summary of the stats
*/
if (data->post_processing_stats != NULL && params->stats & SUM)
    {
    data->search_engine->stats_all_text_render();
    data->post_processing_stats->print_time("Post Processing I/O  :", data->post_processing_stats->disk_input_time);
    data->post_processing_stats->print_time("Post Processing CPU  :", data->post_processing_stats->cpu_time);
    }

fprintf(params->output, "Total elapsed time including startup and shutdown ");
data->stats.print_elapsed_time();
ANT_stats::print_operating_system_process_time();
}

/*
	ANT_FREE()
	-----------
*/
void ant_free(ANT *ant)
{
struct ANT_ant_handle *data = (ANT_ant_handle *)ant;

if (data->post_processing_stats != NULL)
    delete data->post_processing_stats;

delete data->map;
delete data->ranking_function;
delete data->search_engine;
delete [] data->document_list;
delete [] data->filename_list;
delete [] data->mem1;
delete [] data->mem2;

free(ant);
}
