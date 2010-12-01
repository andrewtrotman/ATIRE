/*
	ATIRE_API.C
	-----------
*/
#include "ant_param_block.h"			// FIX THIS
#include "atire_api.h"

#include "memory.h"
#include "disk.h"
#include "stats_time.h"

#include "NEXI_ant.h"
#include "NEXI_term_iterator.h"
#include "query.h"
#include "query_boolean.h"

#include "search_engine.h"
#include "search_engine_readability.h"

#include "stemmer.h"
#include "stemmer_factory.h"

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

#include "assessment_factory.h"
#include "relevant_document.h"
#include "mean_average_precision.h"

#include "search_engine_forum_INEX.h"
#include "search_engine_forum_INEX_efficiency.h"
#include "search_engine_forum_INEX_focus.h"
#include "search_engine_forum_INEX_bep.h"
#include "search_engine_forum_TREC.h"

#include "version.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ATIRE_API::ATIRE_API()
	----------------------
*/
ATIRE_API::ATIRE_API()
{
memory = new ANT_memory;

NEXI_parser = new ANT_NEXI_ant;
boolean_parser = new ANT_query_boolean;
parsed_query = new ANT_query;

search_engine = NULL;
ranking_function = NULL;
stemmer = NULL;

document_list = NULL;
filename_list = NULL;
answer_list = NULL;
mem1 = mem2 = NULL;

assessment_factory = NULL;
assessments = NULL;
map = NULL;
forum_writer = NULL;

query_type_is_all_terms = FALSE;
}

/*
	ATIRE_API::~ATIRE_API()
	-----------------------
*/
ATIRE_API::~ATIRE_API()
{
delete memory;

delete NEXI_parser;
delete boolean_parser;
delete parsed_query;

delete search_engine;
delete stemmer;
delete ranking_function;

delete [] document_list;
delete [] filename_list;
//	delete [] answer_list;		// do not free as allocated within the memory object
delete [] mem1;
delete [] mem2;

delete assessment_factory;
delete [] assessments;
delete map;
delete forum_writer;
}

/*
	ATIRE_API::VERSION()
	--------------------
*/
char *ATIRE_API::version(long *version_number)
{
if (*version_number != NULL)
	*version_number = ANT_version;
return ANT_version_string;
}

/*
	ATIRE_API::MAX()
	----------------
*/
char *ATIRE_API::max(char *a, char *b, char *c)
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
	ATIRE_API::READ_DOCID_LIST()
	----------------------------
*/
char **ATIRE_API::read_docid_list(long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2)
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
	ATIRE_API::OPEN()
	-----------------
*/
long ATIRE_API::open(long type)
{
ANT_search_engine_readability *readable_search_engine;

if (document_list != NULL)
	return 1;		//we're already open;

document_list = read_docid_list(&documents_in_id_list, &filename_list, &mem1, &mem2);
answer_list = (char **)memory->malloc(sizeof(*answer_list) * documents_in_id_list);
if (type & READABILITY_SEARCH_ENGINE)
	{
	search_engine = readable_search_engine = new ANT_search_engine_readability(memory, type & INDEX_IN_MEMORY ? INDEX_IN_MEMORY : INDEX_IN_FILE);
	ranking_function = new ANT_ranking_function_readability(readable_search_engine);
	}
else
	{
	search_engine = new ANT_search_engine(memory, type & INDEX_IN_MEMORY ? INDEX_IN_MEMORY : INDEX_IN_FILE);
	if (search_engine->quantized())
		ranking_function = new ANT_ranking_function_impact(search_engine);
	else
		ranking_function = new ANT_ranking_function_divergence(search_engine);
	}

return 0;		// success
}

/*
	ATIRE_API::LOAD_ASSESSMENTS()
	-----------------------------
*/
long ATIRE_API::load_assessments(char *assessments_filename)
{
if (map != NULL)
	return 1;	// we've already got assessments loaded

assessment_factory = new ANT_assessment_factory(memory, document_list, documents_in_id_list);
assessments = assessment_factory->read(assessments_filename, &number_of_assessments);
map = new ANT_mean_average_precision(memory, assessments, number_of_assessments);

return 0;		// success
}

/*
	ATIRE_API::PARSE_NEXI_QUERY()
	-----------------------------
*/
long ATIRE_API::parse_NEXI_query(char *query)
{
NEXI_parser->set_segmentation(segmentation);
NEXI_parser->parse(parsed_query, query);

return parsed_query->parse_error;
}

/*
	ATIRE_API::SET_RANKING_FUNCTION()
	---------------------------------
	returns:
		0 on success
		1 on failure
*/
long ATIRE_API::set_ranking_function(long function, double p1, double p2)
{
ANT_ranking_function *new_function;

if (search_engine->quantized())
	{
	switch (function)
		{
		case ANT_ANT_param_block::BM25:
		case ANT_ANT_param_block::LMD:
		case ANT_ANT_param_block::LMJM:
		case ANT_ANT_param_block::BOSE_EINSTEIN:
		case ANT_ANT_param_block::DIVERGENCE:
		case ANT_ANT_param_block::INNER_PRODUCT:
			return 1;		// failure because we're a quantized ranking function and we don't have TF values in the index
		}
	}

switch (function)
	{
	case ANT_ANT_param_block::BM25:
		new_function = new ANT_ranking_function_BM25(search_engine, p1, p2);
		break;
	case ANT_ANT_param_block::IMPACT:
		new_function = new ANT_ranking_function_impact(search_engine);
		break;
	case ANT_ANT_param_block::LMD:
		new_function = new ANT_ranking_function_lmd(search_engine, p1);
		break;
	case ANT_ANT_param_block::LMJM:
		new_function = new ANT_ranking_function_lmjm(search_engine, p1);
		break;
	case ANT_ANT_param_block::BOSE_EINSTEIN:
		new_function = new ANT_ranking_function_bose_einstein(search_engine);
		break;
	case ANT_ANT_param_block::DIVERGENCE:
		new_function = new ANT_ranking_function_divergence(search_engine);
		break;
	case ANT_ANT_param_block::TERM_COUNT:
		new_function = new ANT_ranking_function_term_count(search_engine);
		break;
	case ANT_ANT_param_block::INNER_PRODUCT:
		new_function = new ANT_ranking_function_inner_product(search_engine);
		break;
	case ANT_ANT_param_block::ALL_TERMS:
		query_type_is_all_terms = TRUE;
		new_function = new ANT_ranking_function_term_count(search_engine);
		break;
	default: 
		return 1;		// failure, invalid parameter
	}

delete ranking_function;
ranking_function = new_function;

return 0;		// success
}

/*
	ATIRE_API::SET_FORUM()
	----------------------
*/
long ATIRE_API::set_forum(long type, char *output_filename, char *participant_id, char *run_name, long max_results_list_length)
{
ANT_search_engine_forum *output;

switch (type)
	{
	case ANT_ANT_param_block::TREC:
		output = new ANT_search_engine_forum_TREC(output_filename, participant_id, run_name, "RelevantInContext");
		break;
	case ANT_ANT_param_block::INEX:
		output = new ANT_search_engine_forum_INEX(output_filename, participant_id, run_name, "RelevantInContext");
		break;
	case ANT_ANT_param_block::INEX_EFFICIENCY:
		output = new ANT_search_engine_forum_INEX_efficiency(output_filename, participant_id, run_name, max_results_list_length, "RelevantInContext");
		break;
	case ANT_ANT_param_block::INEX_FOCUS:
		output = new ANT_search_engine_forum_INEX_focus(output_filename, participant_id, run_name, "RelevantInContext");
		break;
	case ANT_ANT_param_block::INEX_BEP:
		output = new ANT_search_engine_forum_INEX_bep(output_filename, participant_id, run_name, "RelevantInContext");
		break;
	default:
		return 1;		// failure, invalid parameter
	}

delete forum_writer;
forum_writer = output;
forum_results_list_length = max_results_list_length;

return 0;
}

/*
	ATIRE_API::WRITE_TO_FORUM_FILE()
	--------------------------------
*/
void ATIRE_API::write_to_forum_file(long topic_id)
{
if (forum_writer == NULL)
	return;

search_engine->generate_results_list(document_list, answer_list, hits);
forum_writer->write(topic_id, answer_list, forum_results_list_length > hits ? hits : forum_results_list_length, search_engine, NULL);
}

/*
	ATIRE_API::SET_STEMMER()
	------------------------
*/
long ATIRE_API::set_stemmer(long which_stemmer, long stemmer_similarity, double threshold)
{
ANT_stemmer *new_stemmer;

if (search_engine == NULL)
	return 1;		// fail, we need a search engine object first.

if (which_stemmer == ANT_ANT_param_block::NONE)
	{
	delete stemmer;
	stemmer = NULL;
	}

if ((new_stemmer = ANT_stemmer_factory::get_stemmer(which_stemmer, search_engine, stemmer_similarity, threshold)) == NULL)
	return 1;		// fail, invalid stemmer

delete stemmer;
stemmer = new_stemmer;

return 0;
}

/*
	ATIRE_API::SEARCH()
	-------------------
	returns the number of documents found
*/
long long ATIRE_API::search(char *query, long long top_k)
{
char token[1024];
ANT_NEXI_term_ant *parse_tree, *term_string, **term_list;
ANT_NEXI_term_iterator term;
long terms_in_query, token_length, first_case, current_term;
char *current;

search_engine->stats_initialise();		// if we are command-line then report query by query stats

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	search_engine->init_accumulators(top_k);
#else
	search_engine->init_accumulators();
#endif

/*
	Parse the query and count the number of search terms
*/
parse_tree = NEXI_parser->parse(query);
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
		Sort on local max impact
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
	if (stemmer == NULL || !ANT_islower(*term_string->get_term()->start))		// We don't stem numbers or tag names, of if there is no stemmer
		search_engine->process_one_term_detail(&term_string->term_details, ranking_function);
	else
		{
		token_length = term_string->get_term()->string_length < sizeof(token) - 1 ? term_string->get_term()->string_length : sizeof(token) - 1;
		strncpy(token, term_string->get_term()->start, token_length);
		token[token_length] = '\0';
		search_engine->process_one_stemmed_search_term(stemmer, token, ranking_function);
		}
	}

delete [] term_list;

/*
	Rank the results list
*/
search_engine->sort_results_list(top_k, &hits); // rank

/*
	Conjunctive searching using the disjunctive search engine
*/
if (query_type_is_all_terms)
	hits = search_engine->boolean_results_list(terms_in_query);

/*
	Add the time it took to search to the global stats for the search engine
*/
search_engine->stats_add();

return hits;
}

/*
	ATIRE_API::GENERATE_RESULTS_LIST()
	----------------------------------
*/
char **ATIRE_API::generate_results_list(void)
{
search_engine->generate_results_list(filename_list, answer_list, hits);
return answer_list;
}


/*
	ATIRE_API::GET_WHOLE_DOCUMENT_PRECISION()
	-----------------------------------------
*/
double ATIRE_API::get_whole_document_precision(long topic_id, long metric, long metric_n)
{
double average_precision = 0.0;

/*
	Compute average precision
*/
if (map != NULL)
	{
	if (metric == ANT_ANT_param_block::MAP)
		average_precision = map->average_precision(topic_id, search_engine);
	else if (metric == ANT_ANT_param_block::MAgP)
		average_precision = map->average_generalised_precision_document(topic_id, search_engine);
	else if (metric == ANT_ANT_param_block::RANKEFF)
		average_precision = map->rank_effectiveness(topic_id, search_engine);
	else if (metric == ANT_ANT_param_block::P_AT_N)
		average_precision = map->p_at_n(topic_id, search_engine, metric_n);
	else if (metric == ANT_ANT_param_block::SUCCESS_AT_N)
		average_precision = map->success_at_n(topic_id, search_engine, metric_n);
	/*
		else it might be a focused evaluation metric
	*/
	}

/*
	Return the precision
*/
return average_precision;
}

/*
	ATIRE_API::LOAD_DOCUMENT()
	--------------------------
*/
char *ATIRE_API::load_document(char *buffer, unsigned long *length, long long id)
{
*buffer = '\0';
return search_engine->get_document(buffer, length, id);
}

