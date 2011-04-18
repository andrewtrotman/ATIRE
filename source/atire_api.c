/*
	ATIRE_API.C
	-----------
*/
#include <stdlib.h>
#include "ant_param_block.h"			// FIX THIS
#include "atire_api.h"

#include "memory.h"
#include "disk.h"
#include "bitstring.h"
#include "bitstring_iterator.h"
#include "stats_time.h"
#include "heap.h"

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
#include "ranking_function_kbtfidf.h"
#include "ranking_function_dlh13.h"

#include "assessment_factory.h"
#include "relevant_document.h"
#include "mean_average_precision.h"

#include "search_engine_forum_INEX.h"
#include "search_engine_forum_INEX_efficiency.h"
#include "search_engine_forum_INEX_focus.h"
#include "search_engine_forum_INEX_bep.h"
#include "search_engine_forum_TREC.h"

#include "relevance_feedback.h"
#include "relevance_feedback_factory.h"
#include "memory_index_one_node.h"

#include "search_engine_memory_index.h"
#include "index_document.h"
#include "directory_iterator_object.h"
#include "parser.h"
#include "readability_factory.h"

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
segmentation = FALSE;
parsed_query = new ANT_query;

search_engine = NULL;
ranking_function = NULL;
stemmer = NULL;
feedbacker = NULL;
feedback_documents = feedback_terms = 10;
query_type_is_all_terms = FALSE;
hits = 0;
sort_top_k = LLONG_MAX;

document_list = NULL;
filename_list = NULL;
answer_list = NULL;
documents_in_id_list = 0;
mem1 = mem2 = NULL;

assessment_factory = NULL;
assessments = NULL;
number_of_assessments = 0;
map = NULL;
forum_writer = NULL;
forum_results_list_length = 1500;
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
delete feedbacker;
delete ranking_function;

delete [] document_list;
delete [] filename_list;
//	delete [] answer_list;		// must not be freed as it is allocated within the memory object
delete [] mem1;
delete [] mem2;

delete assessment_factory;
//delete [] assessments;
delete map;
delete forum_writer;
}

/*
	ATIRE_API::VERSION()
	--------------------
*/
char *ATIRE_API::version(long *version_number)
{
if (version_number != NULL)
	*version_number = ANT_version;
return ANT_version_string;
}

/*
	ATIRE_API::MAX()
	----------------
*/
#undef max
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
char **ATIRE_API::read_docid_list(char * doclist_filename, long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2)
{
char *document_list_buffer, *filename_list_buffer;
char **id_list, **current;
char *slish, *slash, *slosh, *start, *dot;

if ((document_list_buffer = ANT_disk::read_entire_file(doclist_filename)) == NULL)
	exit(printf("Cannot open document ID list file '%s'\n", doclist_filename));

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
long ATIRE_API::open(long type, char * index_filename, char * doclist_filename)
{
ANT_search_engine_readability *readable_search_engine;

if (document_list != NULL)
	return 1;		//we're already open;

document_list = read_docid_list(doclist_filename, &documents_in_id_list, &filename_list, &mem1, &mem2);
answer_list = (char **)memory->malloc(sizeof(*answer_list) * documents_in_id_list);
if (type & READABILITY_SEARCH_ENGINE)
	{
	search_engine = readable_search_engine = new ANT_search_engine_readability(memory, type & INDEX_IN_MEMORY ? INDEX_IN_MEMORY : INDEX_IN_FILE);
	search_engine->open(index_filename);

	ranking_function = new ANT_ranking_function_readability(readable_search_engine);
	}
else
	{
	search_engine = new ANT_search_engine(memory, type & INDEX_IN_MEMORY ? INDEX_IN_MEMORY : INDEX_IN_FILE);
	search_engine->open(index_filename);

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
		case ANT_ANT_param_block::KBTFIDF:
		case ANT_ANT_param_block::DLH13:
			return 1;		// failure because we're a quantized ranking function and we don't have TF values in the index
		}
	}

switch (function)
	{
	case ANT_ANT_param_block::BM25:
		new_function = new ANT_ranking_function_BM25(search_engine, p1, p2);
		break;
	case ANT_ANT_param_block::DLH13:
		new_function = new ANT_ranking_function_DLH13(search_engine);
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
	case ANT_ANT_param_block::KBTFIDF:
		new_function = new ANT_ranking_function_kbtfidf(search_engine, p1, p2);
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

if (which_stemmer == ANT_stemmer_factory::NONE)
	new_stemmer = NULL;
else if ((new_stemmer = ANT_stemmer_factory::get_stemmer(which_stemmer, search_engine, stemmer_similarity, threshold)) == NULL)
	return 1;		// fail, invalid stemmer

delete stemmer;
stemmer = new_stemmer;

return 0;
}

/*
	ATIRE_API::SET_FEEDBACKER()
	---------------------------
*/
long ATIRE_API::set_feedbacker(long feedback, long documents, long terms)
{
ANT_relevance_feedback_factory factory;

feedbacker = factory.get_feedbacker(search_engine, feedback);
feedback_documents = documents;
feedback_terms = terms;

return 0;		// success
}

/*
	ATIRE_API::STRING_PAIR_TO_TERM()
	--------------------------------
*/
char *ATIRE_API::string_pair_to_term(char *destination, ANT_string_pair *source, size_t destination_length, long case_fold)
{
long length, first_case;
char *current;

length = source->string_length < destination_length - 1 ? source->string_length : destination_length - 1;
strncpy(destination, source->start, length);
destination[length] = '\0';

/*
	Terms that are in upper-case are tag names for the bag-of-tags approach whereas mixed / lower case terms are search terms
	but as the vocab is in lower case it is necessary to check then convert.
*/
if (case_fold)
	{
	first_case = ANT_islower(*destination);
	for (current = destination; *current != '\0'; current++)
		if (ANT_islower(*current) != first_case)
			{
			strlower(destination);
			break;
			}
	}

return destination;
}

/*
	ATIRE_API::PROCESS_NEXI_QUERY()
	-------------------------------
*/
long ATIRE_API::process_NEXI_query(ANT_NEXI_term_ant *parse_tree)
{
ANT_NEXI_term_ant *term_string;
ANT_NEXI_term_iterator term;
ANT_NEXI_term_ant **term_list;
long terms_in_query, current_term;
long long old_static_prune = 0;

/*
	Load the term details (document frequency, collection frequency, and so on)
	(Load the secondary level dictionary structures and store them in the
	according term's term_details variable).  Also count the number of terms
*/
terms_in_query = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	{
	terms_in_query++;
	/*
		Take the search term (as an ANT_string_pair) and convert into a string
		If you want to know if the term is a + or - term then call term_string->get_sign() which will return 0 if it is not (or +ve or -ve if it is)
	*/
	string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer));
	if (stemmer == NULL || !ANT_islower(*token_buffer))		// so we don't stem numbers or tag names
		search_engine->process_one_term(token_buffer, &term_string->term_details);
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
	If we only have one search term then we can static prune the postings list at top-k
*/
if (terms_in_query == 1)
	{
	old_static_prune = search_engine->set_trim_postings_k(sort_top_k);
	if (old_static_prune < sort_top_k)		// then we were alreay prining at smaller than top_k!
		search_engine->set_trim_postings_k(sort_top_k);
	}

/*
	Process each search term - either stemmed or not.
*/
for (current_term = 0; current_term < terms_in_query; current_term++)
	{
#ifdef NEVER
	printf("%*.*s ", term_list[current_term]->term.length(), term_list[current_term]->term.length(), term_list[current_term]->term.start);
	if (current_term == terms_in_query - 1)
		puts("");
#endif

	term_string = term_list[current_term];
	if (stemmer == NULL || !ANT_islower(*term_string->get_term()->start))		// We don't stem numbers or tag names, of if there is no stemmer
		search_engine->process_one_term_detail(&term_string->term_details, ranking_function);
	else
		{
		string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer));
		search_engine->process_one_stemmed_search_term(stemmer, token_buffer, ranking_function);
		}
	}

delete [] term_list;

/*
	Restore the old static prune value
*/
if (terms_in_query == 1)
	search_engine->set_trim_postings_k(old_static_prune);

return terms_in_query;
}

/*
	ATIRE_API::PROCESS_NEXI_QUERY()
	-------------------------------
*/
long ATIRE_API::process_NEXI_query(char *query)
{
return process_NEXI_query(parsed_query->NEXI_query = NEXI_parser->parse(query));
}

/*
	ATIRE_API::PROCESS_BOOLEAN_QUERY()
	----------------------------------
*/
ANT_bitstring *ATIRE_API::process_boolean_query(ANT_query_parse_tree *root, long *leaves)
{
ANT_bitstring *into, *left, *right;

left = right = NULL;
if (root->boolean_operator == ANT_query_parse_tree::LEAF_NODE)
	{
	(*leaves)++;
	into = new ANT_bitstring;
	into->set_length((long)documents_in_id_list);

	string_pair_to_term(token_buffer, &root->term, sizeof(token_buffer));

	if (stemmer == NULL || !ANT_islower(*token_buffer))		// We don't stem numbers or tag names, or if there is no stemmer
		search_engine->process_one_search_term(token_buffer, ranking_function, into);
	else
		search_engine->process_one_stemmed_search_term(stemmer, token_buffer, ranking_function, into);

	return into;
	}

if (root->left != NULL)
	left = process_boolean_query(root->left, leaves);
if (root->right != NULL)
	right = process_boolean_query(root->right, leaves);

if (left == NULL)
	return right;

if (right == NULL)
	return left;

switch (root->boolean_operator)
	{
	case ANT_query_parse_tree::BOOLEAN_AND:
		left->bit_and(left, right);
		break;
	case ANT_query_parse_tree::BOOLEAN_OR:
		left->bit_or(left, right);
		break;
	case ANT_query_parse_tree::BOOLEAN_NOT:
		left->bit_and_not(left, right);
		break;
	case ANT_query_parse_tree::BOOLEAN_XOR:
		left->bit_xor(left, right);
		break;
	default:
		break;
	}

delete right;

return left;
}

/*
	ATIRE_API::BOOLEAN_TO_NEXI()
	----------------------------
*/
void ATIRE_API::boolean_to_NEXI(ANT_NEXI_term_ant *into, ANT_query_parse_tree *root, long *leaves)
{
if (root->boolean_operator == ANT_query_parse_tree::LEAF_NODE)
	{
	if (*leaves > 0)
		into[*leaves - 1].next = &into[*leaves];
	into[*leaves].parent_path = NULL;

	into[*leaves].next = NULL;
	into[*leaves].sign = 0;
	into[*leaves].path.start = NULL;
	into[*leaves].path.string_length = 0;
	into[*leaves].term = root->term;			// shallow copy
	(*leaves)++;
	}
else
	{
	if (root->left != NULL)
		boolean_to_NEXI(into, root->left, leaves);
	if (root->right != NULL)
		boolean_to_NEXI(into, root->right, leaves);
	}
}

/*
	ATIRE_API::PROCESS_BOOLEAN_QUERY()
	----------------------------------
*/
long ATIRE_API::process_boolean_query(char *query)
{
long answer, added, terms_in_query = 0;
ANT_bitstring *valid_result_set = NULL;
ANT_search_engine_accumulator *accumulator;

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	ANT_search_engine_accumulator **accumulator_pointers;
	long next_relevant_document;
	Heap<ANT_search_engine_accumulator *, ANT_search_engine_accumulator_cmp> *heapk;
#endif

/*
	Parse the query and count the number of search terms.  If there's a parse error
	then don't do the query
*/
boolean_parser->parse(parsed_query, query);
if (parsed_query->parse_error != ANT_query::ERROR_NONE)
	return 0;

/*
	In the case of a purely disjunctive query (OR operators only) we can fall-back to the
	top-k index pruning search engine.  This happens for single term queries too (because they
	are, by definition, conjunctive too).  Now... we do this rather than passing the query
	string directly to the NEXI code because it avoids boolean operators becoming search terms

	In the case of relevance feedback we need the term list for a later disjunctive search
*/
if (parsed_query->subtype == ANT_query::DISJUNCTIVE || feedbacker != NULL)
	{
	terms_in_query = 0;
	parsed_query->NEXI_query = new ANT_NEXI_term_ant[parsed_query->terms_in_query + 1];
	boolean_to_NEXI(parsed_query->NEXI_query, parsed_query->boolean_query, &terms_in_query);

	if (parsed_query->subtype == ANT_query::DISJUNCTIVE)
		{
		answer = process_NEXI_query(parsed_query->NEXI_query);
		if (feedbacker == NULL)
			{
			delete [] parsed_query->NEXI_query;
			parsed_query->NEXI_query = NULL;
			}
		return answer;
		}
	}

/*
	Recurse over the tree
*/
valid_result_set = process_boolean_query(parsed_query->boolean_query, &terms_in_query);

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	if (terms_in_query > 1)			// the heap is already correct if there's only one term in the query
		{
		accumulator = search_engine->results_list->accumulator;
		accumulator_pointers = search_engine->results_list->accumulator_pointers;
		ANT_bitstring_iterator iterator(valid_result_set);

		heapk = search_engine->results_list->heapk;		// re-use the results list heap

		added = 0;
		for (next_relevant_document = iterator.first(); next_relevant_document >= 0; next_relevant_document = iterator.next())
			{
			if (added < sort_top_k)						// just add to the heap
				accumulator_pointers[added] = accumulator + next_relevant_document;
			else if (added > sort_top_k)				// update the heap if this node belongs
				{
				if (accumulator[next_relevant_document].get_rsv() > accumulator_pointers[0]->get_rsv())
					heapk->min_insert(accumulator + next_relevant_document);
				}
			else		// added == sort_top_k			// insert then sort the heap
				{
				accumulator_pointers[added] = accumulator + next_relevant_document;
				heapk->build_min_heap();
				}
			added++;
			}

		search_engine->results_list->results_list_length = added < sort_top_k ? added : sort_top_k;
		}
#else
	for (added = 0; added < documents_in_id_list; added++)
		{
		if (accumulator[added].get_rsv() > 0)
			if (!valid_result_set->unsafe_getbit(added))
				accumulator[added].clear_rsv();
		}
#endif

/*
	Clean up and finish
*/
delete valid_result_set;

return terms_in_query;
}

/*
	ATIRE_API::QUERY_OBJECT_WITH_FEEDBACK_TO_NEXI_QUERY()
	-----------------------------------------------------
*/
void ATIRE_API::query_object_with_feedback_to_NEXI_query(void)
{
ANT_NEXI_term_ant *new_query, *term;
long current_term, current_feedback;

new_query = new ANT_NEXI_term_ant [parsed_query->terms_in_query + parsed_query->feedback_terms_in_query];

/*
	Copy the initial query
*/
for (current_term = 0; current_term < parsed_query->terms_in_query; current_term++)
	{
	new_query[current_term] = parsed_query->NEXI_query[current_term];
	new_query[current_term].next = new_query + current_term + 1;
	}

/*
	Append the feedback terms
*/
for (current_feedback = 0; current_feedback < parsed_query->feedback_terms_in_query; current_feedback++)
	{
	term = new_query + parsed_query->terms_in_query + current_feedback;
	term->next = term + 1;
	term->parent_path = NULL;
	term->path.start = NULL;
	term->sign = 0;
	term->term = parsed_query->feedback_terms[current_feedback]->string;
	}
parsed_query->terms_in_query = parsed_query->terms_in_query + parsed_query->feedback_terms_in_query;
new_query[parsed_query->terms_in_query - 1].next = NULL;


if (parsed_query->type & QUERY_BOOLEAN)
	delete [] parsed_query->NEXI_query;

parsed_query->NEXI_query = new_query;
}

/*
	ATIRE_API::SEARCH()
	-------------------
	returns the number of documents found
*/
long long ATIRE_API::search(char *query, long long top_k, long query_type)
{
/*
	Initialise
*/
sort_top_k = top_k;

search_engine->stats_initialise();

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	search_engine->init_accumulators(top_k);
#else
	search_engine->init_accumulators();
#endif

/*
	Parse and do the query
*/
if (query_type & QUERY_NEXI)
	parsed_query->terms_in_query = process_NEXI_query(query);
else if (query_type & QUERY_BOOLEAN)
	parsed_query->terms_in_query = process_boolean_query(query);
else
	parsed_query->terms_in_query = 0;

/*
	Rank the results list
*/
search_engine->sort_results_list(top_k, &hits); // rank

/*
	Conjunctive searching using the disjunctive search engine
*/
if (query_type_is_all_terms)
	hits = search_engine->boolean_results_list(parsed_query->terms_in_query);

/*
	Blind relevance feedback
*/
if (feedbacker != NULL)
	{
	parsed_query->feedback_terms = feedbacker->feedback(search_engine->results_list, feedback_documents, feedback_terms, &parsed_query->feedback_terms_in_query);
#ifdef NEVER
	printf("\nFEEDBACK TERMS:");
	for (ANT_memory_index_one_node **current = parsed_query->feedback_terms; *current != NULL; current++)
		printf("%*.*s ", (*current)->string.length(), (*current)->string.length(), (*current)->string.start);
	puts("");
#endif
	if (parsed_query->feedback_terms_in_query != 0)
		{
		/*
			Initialise
		*/
		#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
			search_engine->init_accumulators(top_k);
		#else
			search_engine->init_accumulators();
		#endif

		/*
			Generate query, search, and clean up
		*/
		query_object_with_feedback_to_NEXI_query();
		process_NEXI_query(parsed_query->NEXI_query);
		delete [] parsed_query->feedback_terms;
		delete [] parsed_query->NEXI_query;

		/*
			Rank
		*/
		search_engine->sort_results_list(top_k, &hits);
		}
	}

/*
	Add the time it took to search to the global stats for the search engine
*/
search_engine->stats_add();

return hits;
}

/*
	ATIRE_API::RERANK()
	-------------------
*/
void ATIRE_API::rerank(void)
{
ANT_memory_index *indexer;
ANT_search_engine_memory_index *in_memory_index;
long long current, top_n;
char *document_buffer;
const long documents_to_examine = 50;		// load and parse and reindex this many documents;
long long docid;
unsigned long current_document_length;
ANT_directory_iterator_object object;
ANT_readability_factory *readability;
ANT_parser *parser;
ANT_memory *memory;

parser = new ANT_parser(TRUE);
readability = new ANT_readability_factory;
readability->set_measure(ANT_readability_factory::NONE);
readability->set_parser(parser);

indexer = new ANT_memory_index(NULL);
document_buffer = new char [get_longest_document_length()];

top_n = documents_to_examine < search_engine->results_list->results_list_length ? documents_to_examine : search_engine->results_list->results_list_length;

for (current = 0; current < top_n; current++)
	{
	docid = search_engine->results_list->accumulator_pointers[current] - search_engine->results_list->accumulator;
	current_document_length = get_longest_document_length();
	get_document(document_buffer, &current_document_length, docid);

	/*
		Now index the document.
	*/
	object.file = document_buffer;
	index_document(indexer, NULL, TRUE, readability, current, &object);
	}

delete [] document_buffer;
delete readability;
delete parser;
delete indexer;

/*
	turn the index into a search engine.
*/
memory = new ANT_memory;
in_memory_index = new ANT_search_engine_memory_index(indexer, memory);
delete in_memory_index;

delete memory;
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
	ATIRE_API::GET_RELEVANT_DOCUMENT_DETAILS()
	------------------------------------------
*/
long long ATIRE_API::get_relevant_document_details(long long result, long long *docid, double *relevance)
{
if (result < hits)
	{
	*relevance = search_engine->results_list->accumulator_pointers[result]->get_rsv();
	*docid = search_engine->results_list->accumulator_pointers[result] - search_engine->results_list->accumulator;
	}
else
	{
	*relevance = 0;
	*docid = -1;
	}

return *docid;
}

/*
	ATIRE_API::SET_TRIM_POSTINGS_K()
	--------------------------------
*/
long long ATIRE_API::set_trim_postings_k(long long static_prune_point)
{
return search_engine->set_trim_postings_k(static_prune_point);
}

/*
	ATIRE_API::GET_LONGEST_DOCUMENT_LENGTH()
	----------------------------------------
*/
long ATIRE_API::get_longest_document_length(void)
{
return search_engine->get_longest_document_length();
}

/*
	ATIRE_API::GET_DOCUMENT()
	-------------------------
*/
char *ATIRE_API::get_document(char *buffer, unsigned long *length, long long id)
{
return search_engine->get_document(buffer, length, id);
}

/*
	ATIRE_API::STATS_TEXT_RENDER()
	------------------------------
*/
void ATIRE_API::stats_text_render(void)
{
search_engine->stats_text_render();
}

/*
	ATIRE_API::STATS_ALL_TEXT_RENDER()
	----------------------------------
*/
void ATIRE_API::stats_all_text_render(void)
{
search_engine->stats_all_text_render();
}
