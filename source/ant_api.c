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
#include "search_engine_forum_TREC.h"
#include "ant_params.h"
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

/*
 * ANT_ANT_HANDLE
 * --------------
 */
struct ANT_ant_handle
{
ANT_stats stats;
ant_params params;
ANT_search_engine *search_engine;
ANT_search_engine_readability *readable_search_engine;
ANT_mean_average_precision *map = NULL;
ANT_memory memory;
char **document_list, **answer_list, **filename_list;
ANT_relevant_document *assessments = NULL;
long long documents_in_id_list, number_of_assessments;
ANT_ranking_function *ranking_function = NULL;
char *mem1, *mem2;
};

/*
	ANT_INIT()
	----------
*/
ANT *ant_init()
{

}

/*
	READ_DOCID_LIST()
	-----------------
*/
char **read_docid_list(long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2)
{
char *document_list_buffer, *filename_list_buffer;
char **id_list, **current;
char *slish, *slash, *slosh, *start, *dot;

if ((document_list_buffer = ANT_disk::read_entire_file("doclist.aspt")) == NULL)
	exit(printf("Cannot open document ID list file 'doclist.aspt'\n"));

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
	READ_DOCID_LIST()
	-----------------
*/
char **read_docid_list(long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2)
{
char *document_list_buffer, *filename_list_buffer;
char **id_list, **current;
char *slish, *slash, *slosh, *start, *dot;

if ((document_list_buffer = ANT_disk::read_entire_file("doclist.aspt")) == NULL)
	exit(printf("Cannot open document ID list file 'doclist.aspt'\n"));

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
	PERFORM_QUERY()
	---------------
*/
double perform_query(ANT_ANT_param_block *params, ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, char *query, long long *matching_documents, long topic_id, ANT_mean_average_precision *map, ANT_stemmer *stemmer = NULL)
{
ANT_time_stats stats;
long long now, hits;
long did_query, first_case;
char token[1024];
char *token_start, *token_end, *current;
size_t token_length;
ANT_search_engine_accumulator *ranked_list;
double average_precision = 0.0;

/**
 * make the utf8 as the default input encoding
 */
ANT_encoding_utf8 utf8_enc;

search_engine->stats_initialise();		// if we are command-line then report query by query stats

did_query = FALSE;
now = stats.start_timer();
search_engine->init_accumulators();

token_end = query;

while (*token_end != '\0')
	{
	token_start = token_end;
	/*while (!ANT_isalnum(*token_start) && *token_start != '\0')
		token_start++;*/
	while (!(utf8_enc.is_valid_char((unsigned char*)token_start) || ANT_isdigit(*token_start))	&& *token_start != '\0')
		token_start++;
	if (*token_start == '\0')
		break;
	token_end = token_start;
	/*while (ANT_isalnum(*token_end) || *token_end == '+')
		token_end++;*/
	int bytes = 0;
	while (utf8_enc.is_valid_char((unsigned char*)token_end) || ANT_isdigit(*token_end) || *token_end == '+')
		{
		bytes = utf8_enc.howmanybytes();
		if (bytes > 0)
			{
			token_end += bytes;
			bytes = 0;
			if (utf8_enc.lang() == ANT_encoding::CHINESE)
				{
				if (!params->segmentation)
					break;
				/**
				 * TODO Chinese segmentation on query
				 */
				//else
				//
				}
			}
		else
			token_end++;
		}
	strncpy(token, token_start, token_end - token_start);
	token[token_end - token_start] = '\0';
	token_length = token_end - token_start;

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
		average_precision = map->average_generalised_precision(topic_id, search_engine);
	else if (params->metric == ANT_ANT_param_block::RANKEFF)
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

long long ant_search(ANT *ant, char *query)
{

}

void ant_free(ANT *ant)
{
struct ANT_ant_handle *data = (ANT_ant_handle *ant);

delete data->map;
delete data->ranking_function;
delete data->search_engine;
delete [] data->document_list;
delete [] data->filename_list;
delete [] data->mem1;
delete [] data->mem2;
}
