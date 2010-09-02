/*
	ANT.C
	-----
*/
#include <stdio.h>
#include <string.h>
#include "maths.h"
#include "str.h"
#include "memory.h"
#include "ctypes.h"
#include "search_engine.h"
#include "search_engine_readability.h"
#include "search_engine_btree_leaf.h"
#include "mean_average_precision.h"
#include "disk.h"
#include "relevant_document.h"
#include "stats_time.h"
#include "stemmer.h"
#include "stemmer_factory.h"
#include "assessment_factory.h"
#include "search_engine_forum_INEX.h"
#include "search_engine_forum_INEX_efficiency.h"
#include "search_engine_forum_INEX_focus.h"
#include "search_engine_forum_INEX_bep.h"
#include "search_engine_forum_TREC.h"
#include "ant_param_block.h"
#include "version.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_similarity.h"
#include "ranking_function_lmd.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"
#include "ranking_function_term_count.h"
#include "ranking_function_inner_product.h"
#include "parser.h"
#include "NEXI_ant.h"
#include "NEXI_term_iterator.h"
#include "sockets.h"
#include "channel_file.h"
#include "channel_socket.h"
#include "focus_results_list.h"
#include "focus_article.h"
#include "focus_lowest_tag.h"
#include "pdebug.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char *PROMPT = "]";
const long MAX_TITLE_LENGTH = 1024;

/*
	PERFORM_QUERY()
	---------------
*/
double perform_query(ANT_channel *outchannel, ANT_ANT_param_block *params, ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, char *query, long long *matching_documents, long topic_id, ANT_mean_average_precision *map, ANT_stemmer *stemmer, long boolean)
{
ANT_stats_time stats;
long long now, hits, search_time;
long did_query, first_case, token_length;
char *current, token[1024];
double average_precision = 0.0;
ANT_NEXI_ant parser;
ANT_NEXI_term_iterator term;
ANT_NEXI_term_ant *parse_tree, *term_string;
long terms_in_query, current_term;
ANT_NEXI_term_ant **term_list;

search_engine->stats_initialise();		// if we are command-line then report query by query stats

did_query = FALSE;
now = stats.start_timer();

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	search_engine->init_accumulators(params->sort_top_k);
#else
	search_engine->init_accumulators();
#endif

/*
	Parse the query and count the number of search terms
*/
parser.set_segmentation(params->segmentation);
parse_tree = parser.parse(query);
terms_in_query = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	terms_in_query++;

/*
	Load the term details (document frequency, collection frequency, and so on)
	(Load the secondary level dictionary structures and store them in the
	 according term's term_details variable)
*/
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
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
	if (stemmer == NULL || !ANT_islower(*token))		// so we don't stem numbers or tag names
		search_engine->process_one_term(token, &term_string->term_details);
	}
/*
	Prepare an static array structure for sorting
*/
term_list = new ANT_NEXI_term_ant *[terms_in_query];
current_term = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	term_list[current_term++] = term_string;

#ifdef TERM_LOCAL_MAX_IMPACT
/*
 * Sort on local max impact
 */
qsort(term_list, terms_in_query, sizeof(*term_list), ANT_NEXI_term_ant::cmp_local_max_impact);
#else
/*
	Sort on collection frequency works better than document_frequency when tested on the TREC Wall Street Collection
*/
qsort(term_list, terms_in_query, sizeof(*term_list), ANT_NEXI_term_ant::cmp_collection_frequency);
#endif

/*
	Process each search term - either stemmed or not.
*/
for (current_term = 0; current_term < terms_in_query; current_term++)
	{
	term_string = term_list[current_term];
#ifdef TERM_LOCAL_MAX_IMPACT
	dbg_printf("local_max_impact: %d\n", term_string->term_details.local_max_impact);
#endif
	if (stemmer == NULL || !ANT_islower(*term_string->get_term()->start))		// We don't stem numbers or tag names, of if there is no stemmer
		search_engine->process_one_term_detail(&term_string->term_details, ranking_function);
	else
		{
		token_length = term_string->get_term()->string_length < sizeof(token) - 1 ? term_string->get_term()->string_length : sizeof(token) - 1;
		strncpy(token, term_string->get_term()->start, token_length);
		token[token_length] = '\0';
		search_engine->process_one_stemmed_search_term(stemmer, token, ranking_function);
		}

	did_query = TRUE;
	}

delete [] term_list;

/*
	Rank the results list
*/
search_engine->sort_results_list(params->sort_top_k, &hits); // rank

/*
	Boolean Searching
*/
if (boolean)
	hits = search_engine->boolean_results_list(terms_in_query);

/*
	How long did it take?
*/
search_time = stats.stop_timer(now);

/*
	Reporting
*/
if (params->stats & ANT_ANT_param_block::SHORT)
	{
	if (topic_id >= 0)
		printf("Topic:%ld ", topic_id);
	sprintf(token, "Query '%s' found %lld documents in %lld ms", query, hits, stats.time_to_milliseconds(search_time));
	outchannel->puts(token);
//	puts(token);
//	stats.print_time("(", stats.stop_timer(now), ")");
	}

if (did_query && params->stats & ANT_ANT_param_block::QUERY)
	search_engine->stats_text_render();

/*
	Compute average precision
*/
if (map != NULL)
	{
	if (params->metric == ANT_ANT_param_block::MAP)
		average_precision = map->average_precision(topic_id, search_engine);
	else if (params->metric == ANT_ANT_param_block::MAgP)
		average_precision = map->average_generalised_precision_document(topic_id, search_engine);
	else if (params->metric == ANT_ANT_param_block::RANKEFF)
		average_precision = map->rank_effectiveness(topic_id, search_engine);
	else if (params->metric == ANT_ANT_param_block::P_AT_N)
		average_precision = map->p_at_n(topic_id, search_engine, params->metric_n);
	else if (params->metric == ANT_ANT_param_block::SUCCESS_AT_N)
		average_precision = map->success_at_n(topic_id, search_engine, params->metric_n);
	/*
		else it might be a focused evaluation metric
	*/
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
	PROMPT()
	--------
*/
void prompt(ANT_ANT_param_block *params)
{
if (params->queries_filename == NULL && params->port == 0)		// coming from stdin
	printf(PROMPT);
}

/*
	ANT()
	-----
*/
double ant(ANT_search_engine *search_engine, ANT_ranking_function *ranking_function, ANT_mean_average_precision *map, ANT_ANT_param_block *params, char **filename_list, char **document_list, char **answer_list, long boolean)
{
char *print_buffer, *ch;
ANT_stats_time post_processing_stats;
char *command, *query;
long topic_id, line, number_of_queries;
long long hits, result, last_to_list;
long long focused_bytes_parsed, total_focused_bytes_parsed = 0, focused_documents_parsed, total_focused_documents_parsed = 0;
double average_precision, sum_of_average_precisions, mean_average_precision;
ANT_search_engine_forum *output = NULL;
long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;
ANT_stemmer *stemmer = params->stemmer == 0 ? NULL : ANT_stemmer_factory::get_stemmer(params->stemmer, search_engine, params->stemmer_similarity, params->stemmer_similarity_threshold);
long length_of_longest_document;
unsigned long current_document_length;
long long docid;
char *document_buffer, *title_start, *title_end;
long focus_top_k = params->focus_top_k;
ANT_focus_results_list focus_results_list(focus_top_k);
ANT_channel *inchannel, *outchannel;

if (params->port == 0)
	{
	inchannel = new ANT_channel_file(params->queries_filename);		// stdin
	outchannel = new ANT_channel_file();							// stdout
	}
else
	inchannel = outchannel = new ANT_channel_socket(params->port);	// in/out to given port

print_buffer = new char [MAX_TITLE_LENGTH + 1024];

if (params->output_forum == ANT_ANT_param_block::TREC)
	output = new ANT_search_engine_forum_TREC(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == ANT_ANT_param_block::INEX)
	output = new ANT_search_engine_forum_INEX(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == ANT_ANT_param_block::INEX_EFFICIENCY)
	output = new ANT_search_engine_forum_INEX_efficiency(params->output_filename, params->participant_id, params->run_name, params->results_list_length, "RelevantInContext");
else if (params->output_forum == ANT_ANT_param_block::INEX_FOCUS)
	output = new ANT_search_engine_forum_INEX_focus(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
else if (params->output_forum == ANT_ANT_param_block::INEX_BEP)
	output = new ANT_search_engine_forum_INEX_bep(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");

length_of_longest_document = search_engine->get_longest_document_length();

document_buffer = new char [length_of_longest_document + 1];

sum_of_average_precisions = 0.0;
number_of_queries = line = 0;

prompt(params);
for (command = inchannel->gets(); command != NULL; prompt(params), command = inchannel->gets())
	{
	line++;
	/*
		Parsing to get the topic number
	*/
	strip_space_inplace(command);
	if (strcmp(command, ".quit") == 0)
		break;
	if (strncmp(command, ".get ", 5) == 0)
		{
		*document_buffer = '\0';
		if ((current_document_length = length_of_longest_document) != 0)
			{
			search_engine->get_document(document_buffer, &current_document_length, atoll(command + 5));

#ifndef NEVER
/*
	Andrew's experimental code to test focusing
*/
ANT_focus_results_list focus_results_list(2000);		// allow a results list of up-to 2000 focused results
ANT_NEXI_ant parser;
ANT_NEXI_term_iterator term;
ANT_NEXI_term_ant *parse_tree, *term_string;
ANT_focus_lowest_tag focusser(&focus_results_list);
ANT_focus_result *focused_result;
static unsigned char unsigned_marker[] = {0xFF, 0x00};
static char *marker = (char *)unsigned_marker;
long focused_results_list_length;

parse_tree = parser.parse(command + 5);
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	focusser.add_term(&term_string->term);

focused_result = focusser.focus((unsigned char *)document_buffer, &focused_results_list_length);
/*
	Test code to generate INEX FOL format.
*/
{
ANT_search_engine_forum_INEX::focus_to_INEX(document_buffer, focused_result);
printf("%s %lld %lld\n", filename_list[atoll(command + 5)], focused_result->INEX_start, focused_result->INEX_finish - focused_result->INEX_start);
}

sprintf(print_buffer, "%lld", (long long)current_document_length + strlen(marker) + strlen(marker));
outchannel->puts(print_buffer);

outchannel->write(document_buffer, focused_result->start - document_buffer);
outchannel->write(marker, strlen(marker));
outchannel->write(focused_result->start, focused_result->finish - focused_result->start);
outchannel->write(marker, strlen(marker));
outchannel->write(focused_result->finish, current_document_length - (focused_result->finish - document_buffer));
#else

			sprintf(print_buffer, "%lld", current_document_length);
			outchannel->puts(print_buffer);
			outchannel->write(document_buffer, current_document_length);
#endif
			}
		continue;
		}
	if (*command == '\0')
		continue;			// ignore blank lines

	if (have_assessments || params->output_forum != ANT_ANT_param_block::NONE || params->queries_filename != NULL)
		{
		topic_id = atol(command);
		if ((query = strchr(command, ' ')) == NULL)
			exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, command));
		}
	else
		{
		topic_id = -1;
		query = command;
		}

	/*
		Do the query and compute average precision
	*/
	number_of_queries++;

	average_precision = perform_query(outchannel, params, search_engine, ranking_function, query, &hits, topic_id, map, stemmer, boolean);
	sum_of_average_precisions += average_precision;		// zero if we're using a focused metric

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
		How many results to display on the screen.
	*/
	last_to_list = hits > params->results_list_length ? params->results_list_length : hits;

	/*
		Apply focused retrieval to the resuts
	*/
	focused_bytes_parsed = focused_documents_parsed = 0;
	if (params->focussing_algorithm != ANT_ANT_param_block::NONE && length_of_longest_document != 0)
		{
		long long now = post_processing_stats.start_timer();
		long long time_to_focus;
		ANT_focus_result *focused_result;
		ANT_NEXI_ant parser;
		ANT_NEXI_term_iterator term;
		ANT_NEXI_term_ant *parse_tree, *term_string;
		ANT_focus *focusser;
		long focused_hits, passages, current_passage;

		focus_results_list.rewind();

		if (params->focussing_algorithm == ANT_ANT_param_block::RANGE)
			focusser = new ANT_focus_lowest_tag(&focus_results_list);	// closest open tag to first occurence to closest close tag of last.
		else if (params->focussing_algorithm == ANT_ANT_param_block::ARTICLE)
			focusser = new ANT_focus_article(&focus_results_list);		// return the whole article
		else
			focusser = new ANT_focus_article(&focus_results_list);		// default on error

		/*
			Parse (a second time - FIX this in the API) and pass the terms to the focusser
		*/
		parser.set_segmentation(params->segmentation);
		parse_tree = parser.parse(query);
		for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
			focusser->add_term(&term_string->term);

		/*
			Now focus each result until we have the designated number of results in the focused results list
		*/
		focused_hits = 0;
		for (result = 0; result < hits; result++)
			{
			focused_documents_parsed++;
			docid = search_engine->results_list->accumulator_pointers[result] - search_engine->results_list->accumulator;
			current_document_length = length_of_longest_document;

			search_engine->get_document(document_buffer, &current_document_length, docid);
			focused_bytes_parsed += current_document_length;
			focused_result = focusser->focus((unsigned char *)document_buffer, &passages, docid, answer_list[result], search_engine->results_list->accumulator_pointers[result]);

			if ((focused_hits += passages) >= focus_top_k)
				break;

			/*
				Generate output in INEX compatible format
			*/
			if (output != NULL)
				for (current_passage = 0; current_passage < passages; current_passage++)
					{
					ANT_search_engine_forum_INEX::focus_to_INEX(document_buffer, focused_result + current_passage);
//					printf("%lld:%s (%lld-%lld) %f\n", (long long)docid, answer_list[result], focused_result[current_passage].INEX_start, focused_result[current_passage].INEX_finish - focused_result[current_passage].INEX_start, (double)focused_result[current_passage].get_rsv());
					}
			}

		delete focusser;
		post_processing_stats.add_cpu_time(time_to_focus = post_processing_stats.stop_timer(now));
		if (params->stats & ANT_ANT_param_block::QUERY)
			{
			printf("Focused documents    :%lld\n", focused_documents_parsed);
			printf("Focused bytes        :%lld\n", focused_bytes_parsed);
			post_processing_stats.print_time("Focusing time        :", time_to_focus);
			}
		total_focused_bytes_parsed += focused_bytes_parsed;
		total_focused_documents_parsed += focused_documents_parsed;

		/*
			Compute precision using a focused metric
		*/
		if (map != NULL)
			{
			if (params->metric == ANT_ANT_param_block::MAgPf)
				average_precision = map->average_generalised_precision_focused(topic_id, &focus_results_list);
			else if (params->metric == ANT_ANT_param_block::MAiP)
				average_precision = map->average_interpolated_precision(topic_id, &focus_results_list);

			sum_of_average_precisions += average_precision;

			if (params->stats & ANT_ANT_param_block::SHORT)
				printf("Topic:%ld Focused Average Precision:%f\n", topic_id , average_precision);
			}
		}

	/*
		Display the list of results (either to the user or to a run file)
	*/
	if (output != NULL)
		output->write(topic_id, answer_list, last_to_list, search_engine, focused_documents_parsed == 0 ? NULL : &focus_results_list);
	else
		for (result = 0; result < last_to_list; result++)
			{
			docid = search_engine->results_list->accumulator_pointers[result] - search_engine->results_list->accumulator;
			if ((current_document_length = length_of_longest_document) == 0)
				title_start = "";
			else
				{
				search_engine->get_document(document_buffer, &current_document_length, docid);
				if ((title_start = strstr(document_buffer, "<title>")) == NULL)
					if ((title_start = strstr(document_buffer, "<TITLE>")) == NULL)
						title_start = "";
				if (*title_start != '\0')
					{
					title_start += 7;
					if ((title_end = strstr(title_start, "</title>")) == NULL)
						title_end = strstr(title_start, "</TITLE>");
					if (title_end != NULL)
						{
						if (title_end - title_start > MAX_TITLE_LENGTH)
							title_end = title_start + MAX_TITLE_LENGTH;
						*title_end = '\0';
						for (ch = title_start; *ch != '\0'; ch++)
							if (!ANT_isprint(*ch))
								*ch = ' ';
						}
					}
				}
			sprintf(print_buffer, "%lld:%lld:%s %f %s", result + 1, docid, answer_list[result], (double)search_engine->results_list->accumulator_pointers[result]->get_rsv(), title_start);
			outchannel->puts(print_buffer);
			}

	delete [] command;
	}

/* free the allocated forum */
delete output;

/*
	delete the document buffer
*/
delete [] document_buffer;

/*
	Compute Mean Average Precision
*/
mean_average_precision = sum_of_average_precisions / (double)number_of_queries;

/*
	Report MAP
*/
if (map != NULL && params->stats & ANT_ANT_param_block::PRECISION)
	printf("\nProcessed %ld topics (MAP:%f)\n\n", number_of_queries, mean_average_precision);

/*
	Report the summary of the stats
*/
if (params->stats & ANT_ANT_param_block::SUM)
	{
	search_engine->stats_all_text_render();
	if (total_focused_documents_parsed != 0)
		{
		printf("Focused documents    :%lld\n", total_focused_documents_parsed);
		printf("Focused bytes        :%lld\n", total_focused_bytes_parsed);
		post_processing_stats.print_time("Focusing time        :", post_processing_stats.cpu_time);
		}
	}

/*
	Clean up
*/
delete inchannel;
if (inchannel != outchannel)
	delete outchannel;
delete [] print_buffer;

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
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stats stats;
ANT_search_engine *search_engine;
ANT_search_engine_readability *readable_search_engine;
ANT_mean_average_precision *map = NULL;
ANT_memory memory;
long last_param, boolean = FALSE;
ANT_ANT_param_block params(argc, argv);
char **document_list, **answer_list, **filename_list;
ANT_relevant_document *assessments = NULL;
long long documents_in_id_list, number_of_assessments;
ANT_ranking_function *ranking_function = NULL;
char *mem1, *mem2;

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
	search_engine = readable_search_engine = new ANT_search_engine_readability(&memory, params.file_or_memory);
	ranking_function = new ANT_ranking_function_readability(readable_search_engine);
	}
else
	{
	search_engine = new ANT_search_engine(&memory, params.file_or_memory);
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
	else if (params.ranking_function == ANT_ANT_param_block::TERM_COUNT)
		ranking_function = new ANT_ranking_function_term_count(search_engine);
	else if (params.ranking_function == ANT_ANT_param_block::INNER_PRODUCT)
		ranking_function = new ANT_ranking_function_inner_product(search_engine);
	else if (params.ranking_function == ANT_ANT_param_block::ALL_TERMS)
		{
		boolean = TRUE;
		ranking_function = new ANT_ranking_function_term_count(search_engine);
		}
	}
//printf("Index contains %lld documents\n", search_engine->document_count());

search_engine->set_trim_postings_k(params.trim_postings_k);
ant(search_engine, ranking_function, map, &params, filename_list, document_list, answer_list, boolean);

delete map;
delete ranking_function;
delete search_engine;
delete [] document_list;
delete [] filename_list;
delete [] mem1;
delete [] mem2;

printf("Total elapsed time including startup and shutdown ");
stats.print_elapsed_time();
ANT_stats::print_operating_system_process_time();
return 0;
}
