/*
	ANT.C
	-----
	call GetSystemTimes() at the end and compare.
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
#include "thesaurus_engine.h"

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
double perform_query(ANT_ANT_param_block *params, ANT_search_engine *search_engine, char *query, long long *matching_documents, long topic_id = -1, ANT_mean_average_precision *map = NULL)
{
ANT_time_stats stats;
long long now, hits;
long did_query, first_case;
char token[1024];
char *token_start, *token_end, *current;
size_t token_length;
ANT_search_engine_accumulator *ranked_list;
double average_precision = 0.0;
ANT_stemmer *stemmer;

/**
 * make the utf8 as the default input encoding
 */
ANT_encoding_utf8 utf8_enc;

/*
	if we're stemming then create the stemmer object
*/
stemmer = params->stemmer == 0 ? NULL : ANT_stemmer_factory::get_stemmer(params->stemmer, search_engine);

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
	if (stemmer == NULL)
		search_engine->process_one_search_term(token);
	else
		search_engine->process_one_stemmed_search_term(stemmer, token);

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
	Clean up
*/
delete stemmer;

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
double ant(ANT_search_engine *search_engine, ANT_mean_average_precision *map, ANT_ANT_param_block *params, char **document_list, char **answer_list)
{
char *query;
long topic_id, line, number_of_queries;
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

	if (have_assessments || params->output_forum != ANT_ANT_param_block::NONE)
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
	search_engine->stats_all_text_render();

/*
	And finally report MAP
*/
return mean_average_precision;
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
char **read_docid_list(long long *documents_in_id_list)
{
char *document_list_buffer;			// this is leaked!!!!
char **id_list, **current;
char *slish, *slash, *slosh, *start, *dot;

if ((document_list_buffer = ANT_disk::read_entire_file("doclist.aspt")) == NULL)
	exit(printf("Cannot open document ID list file 'doclist.aspt'\n"));

id_list = ANT_disk::buffer_to_list(document_list_buffer, documents_in_id_list);

#ifdef NEVER
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
		dot = strchr(start, '.');
		*dot = '\0';
		*current = start == *current ? *current : start + 1;		// +1 to skip over the '/'
		}
	}
#endif
return id_list;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_search_engine *search_engine;
ANT_mean_average_precision *map = NULL;
ANT_memory memory;
long last_param;
ANT_ANT_param_block params(argc, argv);
char **document_list, **answer_list;
ANT_relevant_document *assessments = NULL;
long long documents_in_id_list, number_of_assessments;

last_param = params.parse();

if (params.logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

document_list = read_docid_list(&documents_in_id_list);

if (params.assessments_filename != NULL)
	{
	ANT_assessment_factory factory(&memory, document_list, documents_in_id_list);
	assessments = factory.read(params.assessments_filename, &number_of_assessments);
	map = new ANT_mean_average_precision(&memory, assessments, number_of_assessments);
	}

answer_list = (char **)memory.malloc(sizeof(*answer_list) * documents_in_id_list);

if (params.readability)
	search_engine = new ANT_search_engine_readability(&memory);
else if (params.thesaurus)
	search_engine = new thesaurus_engine(&memory, params.thesaurus_threshold);
else
	search_engine = new ANT_search_engine(&memory);
//printf("Index contains %lld documents\n", search_engine->document_count());

ant(search_engine, map, &params, document_list, answer_list);

#ifdef FIT_BM25
/*
	This code can be used for optimising the BM25 parameters.
*/
if (argc == 4)
	{
	double map;
	FILE *outfile;
	extern double BM25_k1;
	extern double BM25_b;

	outfile = fopen(argv[3], "wb");
	fprintf(outfile, "%f ", 0.0);
	for (BM25_b = 0.1; BM25_b < 1.0; BM25_b += 0.1)
		fprintf(outfile, "%f ", BM25_b);
	fprintf(outfile, "\n");

	for (BM25_k1 = 0.1; BM25_k1 < 4.0; BM25_k1+= 0.1)
		{
		fprintf(outfile, "%f ", BM25_k1);
		for (BM25_b = 0.1; BM25_b < 1.0; BM25_b += 0.1)
			{
			map = batch_ant(argv[1], argv[2]);
			fprintf(outfile, "%f ", map);
			}
		fprintf(outfile, "\n");
		}
	}
	fclose(outfile);
#endif

delete map;
return 0;
}


