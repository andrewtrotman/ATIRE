/*
	ATIRE_API.C
	-----------
*/
#include <stdlib.h>
#include <sstream>
#include "maths.h"
#include "numbers.h"
#include "ant_param_block.h"
#include "atire_api.h"

#include "btree_iterator.h"
#include "compress.h"
#include "impact_header.h"
#include "heap.h"

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
#include "search_engine_result_id_iterator.h"

#include "stemmer.h"
#include "stemmer_factory.h"
#include "stem_stemmer.h"

#include "thesaurus.h"

#include "ranking_function_factory.h"
#include "ranking_function_readability.h"
#include "ranking_function_divergence.h"
#include "ranking_function_impact.h"
#include "ranking_function_topsig_positive.h"
#include "ranking_function_topsig_negative.h"
#include "ranking_function_pregen.h"

#include "assessment_factory.h"
#include "relevant_document.h"
#include "evaluator.h"

#include "search_engine_forum_INEX.h"
#include "search_engine_forum_INEX_efficiency.h"
#include "search_engine_forum_INEX_focus.h"
#include "search_engine_forum_INEX_bep.h"
#include "search_engine_forum_TREC.h"

#include "relevance_feedback.h"
#include "relevance_feedback_topsig.h"
#include "relevance_feedback_factory.h"
#include "memory_index_one_node.h"

#include "search_engine_memory_index.h"
#include "index_document.h"
#include "index_document_topsig.h"
#include "index_document_topsig_signature.h"
#include "directory_iterator_object.h"
#include "parser.h"
#include "readability_factory.h"

#include "search_engine_accumulator.h"

#include "version.h"

#include "quantum.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
 * This created just for the purpose of automatic atire api library checking
 * for configure.ac, it must use AC_LANG(C++) to force library check with g++
 */
extern "C" {
	void atire_api_check() {}
}

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
expander_tf = NULL;
expander_query = NULL;
more_like_term_chooser = NULL;
feedbacker = NULL;
feedback_ranking_function = NULL;
feedback_documents = feedback_terms = 10;
feedback_lambda = 0.5;
feedback_mode = FEEDBACK_REPLACE;
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
evaluator = NULL;
forum_writer = NULL;
forum_results_list_length = 1500;

pregens = NULL;
pregen_count = 0;
document_indexer = new ANT_index_document;

topsig_width = 0;
topsig_density = 0;
topsig_globalstats = NULL;
topsig_signature = NULL;
topsig_positive_ranking_function = NULL;
topsig_negative_ranking_function = NULL;

processing_strategy = ANT_ANT_param_block::TERM_AT_A_TIME;
impact_header_buffers = NULL;
number_of_impact_headers = 0;
}

/*
	ATIRE_API::~ATIRE_API()
	-----------------------
*/
ATIRE_API::~ATIRE_API()
{
delete NEXI_parser;
delete boolean_parser;
delete parsed_query;

delete search_engine;
delete stemmer;
delete expander_tf;
delete expander_query;
delete feedbacker;
delete more_like_term_chooser;
delete ranking_function;

delete [] document_list;
delete [] filename_list;
//	delete [] answer_list;		// must not be freed as it is allocated within the memory object
delete [] mem1;
delete [] mem2;

delete assessment_factory;
//delete [] assessments;
delete evaluator;
delete forum_writer;

delete [] pregens;

delete memory;
delete document_indexer;

delete topsig_globalstats;
delete topsig_signature;
delete topsig_positive_ranking_function;
delete topsig_negative_ranking_function;
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
	ATIRE_API::READ_DOCID_LIST()
	----------------------------

	Read the docid list from the given file and return it, or NULL if the file could
	not be read.
*/
char **ATIRE_API::read_docid_list(char *doclist_filename, long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2)
{
char *document_list_buffer, *filename_list_buffer;
char **id_list, **current;
char *slish, *slash, *slosh, *start, *dot;

if ((document_list_buffer = ANT_disk::read_entire_file(doclist_filename)) == NULL)
	return NULL;

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
	start = ANT_max(slish, slash, slosh);		// get the posn of the final dir seperator (or the start of the string)
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
long ATIRE_API::open(long type, char *index_filename, char *doclist_filename, long quantize, long long quantization_bits)
{
ANT_search_engine_readability *readable_search_engine;

if (document_list != NULL)
	return 1;		//we're already open;

#ifndef FILENAME_INDEX
document_list = read_docid_list(doclist_filename, &documents_in_id_list, &filename_list, &mem1, &mem2);
if (document_list == NULL)
	return 1;		//document list could not be read
#endif

answer_list = (char **)memory->malloc(sizeof(*answer_list) * documents_in_id_list);
if (type & READABILITY_SEARCH_ENGINE)
	{
	search_engine = readable_search_engine = new ANT_search_engine_readability(memory, type & INDEX_IN_MEMORY ? INDEX_IN_MEMORY : INDEX_IN_FILE);
	if (search_engine->open(index_filename)==0)
		return 1; //fail

	/*
		Makes no sense to quantize readability based ranking ... at least it doesn't now
	*/
	delete ranking_function;
	ranking_function = new ANT_ranking_function_readability(readable_search_engine, false, 0);
	}
else
	{
	search_engine = new ANT_search_engine(memory, type & INDEX_IN_MEMORY ? INDEX_IN_MEMORY : INDEX_IN_FILE);
	if (search_engine->open(index_filename) == 0)
		return 1; //fail

	/*
		If it's already quantized, then ignore what the
		user says about quantization
	*/
	delete ranking_function;
	if (search_engine->quantized())
		ranking_function = new ANT_ranking_function_impact(search_engine, false, -1);
	else
		ranking_function = new ANT_ranking_function_divergence(search_engine, quantize, quantization_bits);
	}

return 0;		// success
}

/*
	ATIRE_API::LOAD_PREGEN()
	------------------------

	Load a pregen with the given filename. Return non-zero on success.
*/
long ATIRE_API::load_pregen(const char *pregen_filename)
{
if (pregens == NULL)
	pregens = new ANT_pregen[MAX_PREGEN_COUNT];

if (pregens[pregen_count].read(pregen_filename))
	{
	pregen_count++;

	return 1;
	}

return 0;
}

/*
	ATIRE_API::LOAD_TOPSIG()
	------------------------
	returns true on succcess and false of failure
*/
long ATIRE_API::load_topsig(long width, double density, char *global_stats_file)
{
delete topsig_globalstats;
delete topsig_signature;

topsig_width = width;
topsig_density = density;
topsig_globalstats = new ANT_index_document_topsig(ANT_memory_index::NONE, width, density, global_stats_file);
topsig_signature = new ANT_index_document_topsig_signature(width, density);

if (topsig_positive_ranking_function == NULL)
	topsig_positive_ranking_function = new ANT_ranking_function_topsig_positive(search_engine, width, false, -1);

if (topsig_negative_ranking_function == NULL)
	topsig_negative_ranking_function = new ANT_ranking_function_topsig_negative(search_engine, width, false, -1);

return true;
}

/*
	ATIRE_API::LOAD_ASSESSMENTS()
	-----------------------------
*/
long ATIRE_API::load_assessments(char *assessments_filename, ANT_evaluator *evaluator)
{
if (evaluator->is_initialised)
	return 1; // we've already got assessments loaded

/*
	If we haven't already said to use an evaluation, use MAP by default
*/
if (evaluator->number_evaluations_used == 0)
	evaluator->add_evaluation("MAP");

assessment_factory = new ANT_assessment_factory(memory, document_list, documents_in_id_list);
assessments = assessment_factory->read(assessments_filename, &number_of_assessments);
if (assessments == NULL)
	exit(printf("Can't open assessment file:%s\n", assessments_filename));
evaluator->initialise(memory, assessments, number_of_assessments);

return 0;		// success
}

/*
	ATIRE_API::PARSE_NEXI_QUERY()
	-----------------------------
*/
ANT_NEXI_term_ant *ATIRE_API::parse_NEXI_query(char *query)
{
NEXI_parser->set_thesaurus(expander_query);
NEXI_parser->set_segmentation(segmentation);
parsed_query->NEXI_query = NEXI_parser->parse(query);

return parsed_query->NEXI_query;
}

/*
	ATIRE_API::SET_RANKING_FUNCTION_PREGEN()
	---------------------------------------
	Set the ranking function to use the specified pregenerated ranking from those
	which have been preloaded with load_pregen().

	returns:
		0 on success
		1 on failure - Pregen with that field name wasn't there to be found

	On failure, the API is left unchanged.
*/
long ATIRE_API::set_ranking_function_pregen(const char *fieldname, double p1)
{
long current;

for (current = 0; current < pregen_count; current++)
	if (strcmp(pregens[current].field_name, fieldname) == 0)
		{
		delete ranking_function;
		ranking_function = new ANT_ranking_function_pregen(search_engine, false, -1, &pregens[current], (long)p1);

		return 0;		// success
		}

return 1;
}

ANT_pregen *ATIRE_API::get_pregen()
{
/* assume the first pregen is going to be used for accumulator initialisation */
if (pregen_count > 0)
	return &pregens[0];

return NULL;
}

/*
	ATIRE_API::SET_RANKING_FUNCTION()
	---------------------------------
	returns:
		0 on success
		1 on failure
*/
long ATIRE_API::set_ranking_function(long long function, long quantization, long long quantization_bits, double p1, double p2, double p3)
{
ANT_ranking_function *new_function;

if ((new_function = decode_ranking_function(function, quantization, quantization_bits, p1, p2, p3)) == NULL)
	return 1;

delete ranking_function;
ranking_function = new_function;
ranking_function_id = function;

return 0;
}

/*
	ATIRE_API::SET_FEEDBACK_RANKING_FUNCTION()
	------------------------------------------
	returns:
		0 on success
		1 on failure
*/
long ATIRE_API::set_feedback_ranking_function(long long function, long quantization, long long quantization_bits, double p1, double p2, double p3)
{
ANT_ranking_function *new_function;

if (function == ANT_ANT_param_block::NONE)
	new_function = NULL;
else
	if ((new_function = decode_ranking_function(function, quantization, quantization_bits, p1, p2, p3)) == NULL)
		return 1;

feedback_ranking_function = new_function;

return 0;
}

/*
	ATIRE_API::DECODE_RANKING_FUNCTION()
	------------------------------------
	returns:
		NULL on failure
*/
ANT_ranking_function *ATIRE_API::decode_ranking_function(long long function, long quantization, long long quantization_bits, double p1, double p2, double p3)
{
long current;

if (search_engine->quantized())
	for (current = 0; ANT_list_of_rankers[current].name != NULL; current++)
		if (function == ANT_list_of_rankers[current].id)
			if ((ANT_list_of_rankers[current].flags & ANT_ranking_function_factory_object::QUANTABLE) != 0)
				{
				puts("Cannot set ranking function as the index is quantized");
				return NULL;		// failure because we're a quantized ranking function and we don't have TF values in the index
				}

if (function == ANT_ranking_function_factory_object::ALL_TERMS)
	query_type_is_all_terms = true;

return ANT_ranking_function_factory::get_searching_ranker(search_engine, function, quantization, quantization_bits, p1, p2, p3);
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

#ifdef FILENAME_INDEX
	forum_writer->write(topic_id, forum_results_list_length > hits ? hits : forum_results_list_length, search_engine, NULL);
#else
	search_engine->generate_results_list(document_list, answer_list, hits);
	forum_writer->write(topic_id, answer_list, forum_results_list_length > hits ? hits : forum_results_list_length, search_engine, NULL);
#endif
}

/*
	ATIRE_API::SET_STEMMER()
	------------------------
	returns 0 on success and 1 on failure
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
	ATIRE_API::SET_INPLACE_QUERY_EXPANSION()
	----------------------------------------
	returns 0 on success and 1 on failure

	This expander is used like a stemmer to increase TF counts.
	see set_query_expander() for classic query expansion
*/
long ATIRE_API::set_inplace_query_expansion(ANT_thesaurus *expander)
{
this->expander_tf = expander;

return 0;
}

/*
	ATIRE_API::SET_QUERY_EXPANSION()
	--------------------------------
	returns 0 on success and 1 on failure
*/
long ATIRE_API::set_query_expansion(ANT_thesaurus *expander)
{
this->expander_query = expander;

return 0;
}

/*
	ATIRE_API::SET_FEEDBACKER()
	---------------------------
*/
long ATIRE_API::set_feedbacker(long feedback, long documents, long terms)
{
ANT_relevance_feedback_factory factory;
ANT_relevance_feedback_topsig *topsig_rf;

feedbacker = factory.get_feedbacker(search_engine, feedback);
feedback_documents = documents;
feedback_terms = terms;

if ((topsig_rf = dynamic_cast<ANT_relevance_feedback_topsig *>(feedbacker)) != NULL)
	topsig_rf->set_topsig_parameters(topsig_width, topsig_density, search_engine->term_count(), topsig_globalstats);

return 0;		// success
}

/*
	ATIRE_API::STRING_PAIR_TO_TERM()
	--------------------------------
*/
char *ATIRE_API::string_pair_to_term(char *destination, ANT_string_pair *source, size_t destination_length, long case_fold)
{
unsigned long character;
long has_non_upper = false;
char *current, *dest_current;
size_t dest_remain, length;

/*
	Terms that are in upper-case are tag names for the bag-of-tags approach whereas mixed / lower case terms are search terms
	but as the vocab is in lower case it is necessary to check then convert.
*/
if (case_fold)
	{
	/*
		Does this term have characters which are not uppercase? If so, it's not a tag name
	*/
	current = source->start;
	while (current < source->start + source->string_length)
		{
		if (!utf8_isupper(utf8_to_wide(current)))
			{
			has_non_upper = true;
			break;
			}

		current += utf8_bytes(current);
		}

	if (has_non_upper)
		{
		/*
			This is a term, not a tag name. Normalize
		*/
		current = source->start;
		dest_current = destination;
		dest_remain = destination_length - 1; /* Leave room for null terminator */
		while (current < source->start + source->string_length)
			{
			character = utf8_to_wide(current);
			current += utf8_bytes(current);

			if (!ANT_UNICODE_normalize_lowercase_toutf8(&dest_current, &dest_remain, character))
				break; //Ran out of room in destination
			}

		*dest_current = '\0';

		return destination;
		}

	/*
		Otherwise we can just copy it, fall through to use as-is.
	*/
	}

/*
	We can just do a straight copy in
*/
length = source->string_length < destination_length - 1 ? source->string_length : destination_length - 1;
strncpy(destination, source->start, length);
destination[length] = '\0';

return destination;
}

#ifdef IMPACT_HEADER
/*
	ATIRE_API::SEARCH_QUANTUM_AT_A_TIME()
	-------------------------------------
*/
void ATIRE_API::search_quantum_at_a_time(ANT_NEXI_term_ant **term_list, long long terms_in_query, ANT_ranking_function *ranking_function)
{
long long current_term, total_quantums = 0, processed_quantums;
ANT_NEXI_term_ant *term_string;
ANT_quantum the_quantum;
ANT_max_quantum *current_max = NULL;
ANT_impact_header *current_impact_header = NULL;
long long trim_postings_k = search_engine->get_trim_postings_k();

//
// read all the postings and get the decompressed impact headers
//
heap_items = max_remaining_quantum = processed_postings = 0;
for (current_term = 0, total_quantums = 0; current_term < terms_in_query; current_term++)
	{
	term_string = term_list[current_term];

	// after calling the function, the header_info and impact header are stored in the impact_header_buffers[current_term]
	search_engine->read_and_decompress_for_one_impact_header(&term_string->term_details, raw_postings_buffer, impact_header_buffers[current_term]);
	//make sure the term was found in the dictionary
	if (term_string->term_details.local_collection_frequency != 0)
		{
		impact_header_buffers[current_term]->impact_value_ptr = impact_header_buffers[current_term]->impact_value_start;
		impact_header_buffers[current_term]->doc_count_ptr = impact_header_buffers[current_term]->doc_count_start;
		impact_header_buffers[current_term]->impact_offset_ptr = impact_header_buffers[current_term]->impact_offset_start;

		total_quantums += impact_header_buffers[current_term]->the_quantum_count;

		// set up the heap of max quantum
		max_quantums[heap_items].current_max_quantum = *impact_header_buffers[current_term]->impact_value_ptr;
		max_quantums[heap_items].quantum_count = impact_header_buffers[current_term]->the_quantum_count;
		max_quantums[heap_items].the_impact_header = impact_header_buffers[current_term];
		max_quantums[heap_items].term_details = &term_string->term_details;
		max_quantums[heap_items].prescalar = term_string->tf_weight;
		max_quantums[heap_items].postscalar = term_string->rsv_weight;
		max_quantums[heap_items].query_frequency = term_string->query_frequency;
		max_quantums_pointers[heap_items] = &max_quantums[heap_items];
		max_remaining_quantum += max_quantums[heap_items].current_max_quantum;
		heap_items++;
		}
	}

// set the correct number of items to keep track in the heap and build the heap
quantum_heap->set_size(heap_items);
quantum_heap->build_max_heap();

#ifdef PRINT_QUANTUM_STATS
printf("total quantums: %lld\n", total_quantums);
#endif

//
// process the quantum in descending order of impact values
//
the_quantum.accumulator = search_engine->results_list;
the_quantum.trim_point = search_engine->trim_postings_k;
processed_quantums = 0;
while (heap_items > 0)
	{
	//
	// process the current max quantum in the heap
	//
	current_max = max_quantums_pointers[0];
	current_impact_header = current_max->the_impact_header;

	the_quantum.impact_value = *current_impact_header->impact_value_ptr;
	the_quantum.doc_count = *current_impact_header->doc_count_ptr;
	the_quantum.offset = *current_impact_header->impact_offset_ptr;
	the_quantum.term_details = current_max->term_details;
	the_quantum.tf = the_quantum.impact_value;
	the_quantum.prescalar = current_max->prescalar;
	the_quantum.postscalar = current_max->postscalar;

	//printf("max remaining quantum: %lld, diff of k and k+1: %lld\n", max_remaining_quantum, search_engine->results_list->get_diff_k_and_k_plus_1());
	if ((early_termination & ANT_ANT_param_block::QUANTUM_STOP_DIFF) && search_engine->results_list->heap_is_full() && max_remaining_quantum < search_engine->results_list->get_diff_k_and_k_plus_1())
		{
		if (early_termination & ANT_ANT_param_block::QUANTUM_STOP_DIFF_SMALLEST)
			{
			if (max_remaining_quantum < search_engine->results_list->get_smallest_diff_amoung_the_top())
				{
				//printf(">>>>>>> QUANTUM_STOP_DIFF_SMALLEST stopped\n");
				break;
				}
			}
		else if (early_termination & ANT_ANT_param_block::QUANTUM_STOP_DIFF_LARGEST)
			{
			if (max_remaining_quantum < search_engine->results_list->get_diff_between_largest_and_second_largest())
				{
				//printf(">>>>>>> QUANTUM_STOP_DIFF_LARGEST stopped\n");
				break;
				}
			}
		else // the QUANTUM_STOP_DIFF case
			{
			//printf(">>>>>>> QUANTUM_STOP_DIFF stopped\n");
			break;
			}
		}

	processed_postings += the_quantum.doc_count;
	// partial process of the last quantum based on the command line option "-K"
	if (processed_postings > trim_postings_k)
		the_quantum.doc_count = (ANT_compressable_integer)(processed_postings - trim_postings_k);

	search_engine->read_and_decompress_for_one_quantum(current_max->term_details, raw_postings_buffer, current_impact_header, &the_quantum, one_decompressed_quantum);
	the_quantum.the_quantum = one_decompressed_quantum;
	the_quantum.quantum_end = one_decompressed_quantum + the_quantum.doc_count;

	processed_quantums++;
	ranking_function->relevance_rank_one_quantum(&the_quantum);

	// static pruning base on the command line option "-K"
	if (processed_postings >= trim_postings_k)
		{
		//printf(">>>>>>> trim_postings_k stopped\n");
		break;
		}

	//
	// update the pointers and insert the current max quantum for the term into the heap
	//
	current_max->quantum_count--;
	total_quantums--;
	// re-calculate the maximum remaining quantum
	max_remaining_quantum -= current_max->current_max_quantum;
	if (current_max->quantum_count > 0)
		{
		current_impact_header->impact_value_ptr++;
		current_impact_header->impact_offset_ptr++;
		current_impact_header->doc_count_ptr++;
		current_max->current_max_quantum = *current_impact_header->impact_value_ptr;
		max_remaining_quantum += current_max->current_max_quantum;
		// update the heap
		quantum_heap->max_update_maximum(current_max);
		}
	else
		{
		// pseudo-remove the item
		current_max->current_max_quantum = 0;
		quantum_heap->max_update_maximum(current_max);
		heap_items--;
		}
	}

#ifdef PRINT_QUANTUM_STATS
printf("processed quantums: %lld\n", processed_quantums);
printf("processed postings: %lld\n", processed_postings);
#endif
}
#endif // end of #ifdef IMPACT_HEADER

/*
	ATIRE_API::SEARCH_TERM_AT_A_TIME()
	----------------------------------
*/
void ATIRE_API::search_term_at_a_time(ANT_NEXI_term_ant **term_list, long long terms_in_query, ANT_ranking_function *ranking_function, ANT_thesaurus *expander_tf, ANT_stemmer *stemmer)
{
long long current_term;
ANT_NEXI_term_ant *term_string;

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

	if (!ANT_islower(*term_string->get_term()->start))		// We don't stem (or expand) numbers and tag names
		search_engine->process_one_term_detail(&term_string->term_details, ranking_function, term_string->tf_weight, term_string->rsv_weight, term_string->query_frequency);
	else
		{
		if (expander_tf != NULL)
			{
			string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer), true);
			search_engine->process_one_thesaurus_search_term(expander_tf, stemmer, token_buffer, ranking_function, term_string->tf_weight, term_string->rsv_weight, term_string->query_frequency);
			}
		else if (stemmer != NULL)
			{
			string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer), true);
			search_engine->process_one_stemmed_search_term(stemmer, token_buffer, ranking_function, term_string->tf_weight, term_string->rsv_weight, term_string->query_frequency);
			}
		else
			search_engine->process_one_term_detail(&term_string->term_details, ranking_function, term_string->tf_weight, term_string->rsv_weight, term_string->query_frequency);
		}
	}
}

/*
	ATIRE_API::PROCESS_NEXI_QUERY()
	-------------------------------
*/
long ATIRE_API::process_NEXI_query(ANT_NEXI_term_ant *parse_tree, ANT_ranking_function *ranking_function, double fake_terms_in_query)
{
double computed_query_length;
ANT_NEXI_term_ant *term_string;
ANT_NEXI_term_iterator term;
ANT_NEXI_term_ant **term_list;
long terms_in_query, current_term;
long long old_static_prune = 0;
long can_sort;
long long sum_of_document_frequencies = 0;

/*
	Load the term details (document frequency, collection frequency, and so on)
	(Load the secondary level dictionary structures and store them in the
	according term's term_details variable).  Also count the number of terms
*/
can_sort = false;
terms_in_query = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	{
	terms_in_query++;

	/*
		Take the search term (as an ANT_string_pair) and convert into a string
		If you want to know if the term is a + or - term then call term_string->get_sign() which will return 0 if it is not (or +ve or -ve if it is)
	*/
	string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer), true);

	if ((stemmer == NULL && expander_tf == NULL) || !ANT_islower(*token_buffer))		// so we don't stem numbers or tag names
		{
		search_engine->process_one_term(token_buffer, &term_string->term_details);
		sum_of_document_frequencies += ANT_min(term_string->term_details.local_document_frequency, search_engine->get_trim_postings_k());
		can_sort = true;
		}
	}

/*
   Check if the number of terms exceed the limit
 */
if (terms_in_query > MAX_ALLOWED_TERMS_IN_QUERY)
	exit(printf("Exceeded the allowed number of %d terms per query\n", MAX_ALLOWED_TERMS_IN_QUERY));

/*
	Prepare an array structure for sorting
*/
term_list = new ANT_NEXI_term_ant *[terms_in_query];
current_term = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	term_list[current_term++] = term_string;

/*
	Tell the search engine how many terms are in the query (because this is used in Language Models for ranking)
*/
if (fake_terms_in_query <= 0)
	{								// else we've already set the values due to the feedbacker
	current_term = 0;
	computed_query_length = 0;
	for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
		if (ranking_function_id == ANT_ranking_function_factory_object::PUURULA_IDF)
			{
			computed_query_length += term_list[current_term]->query_frequency = log(1.0 + term_list[current_term]->query_frequency / terms_in_query) * log((double)get_document_count() / (double)term_string->term_details.global_document_frequency);
			current_term++;
			}
		else
			computed_query_length += 1.0;

	search_engine->results_list->set_term_count(computed_query_length);
	}
else
	search_engine->results_list->set_term_count(fake_terms_in_query);


#ifdef TERM_LOCAL_MAX_IMPACT
	/*
		Sort on local max impact
	*/
	if (can_sort)
		qsort(term_list, terms_in_query, sizeof(*term_list), ANT_NEXI_term_ant::cmp_local_max_impact);
#else
	/*
		Sort on collection frequency works better than document_frequency when tested on the TREC Wall Street Collection
	*/
	if (can_sort)
		qsort(term_list, terms_in_query, sizeof(*term_list), ANT_NEXI_term_ant::cmp_collection_frequency);
#endif

#ifdef TWO_D_ACCUMULATORS
	/*
		Now we take a slight divergence.  We know the number of documents (N),
		the numnber of query terms (Q), and the length of each postings list (L).  From this we
		can compute B (the optimal width of the 2D accumulators) as B = sqrt(xN / yLQ), where x is the
		cost of initialising a column flag and y is the cost of initialising an accumulator.
		see: Jia, X.-F., Trotman, A., Keefe, R.A. (2010), Efficient Accumulator Initialisation, Proceedings of the 15th Australasian Document Computing Symposium (ADCS 2010)
	*/
	#ifdef TWO_D_ACCUMULATORS_POW2_WIDTH
		search_engine->set_accumulator_width(ANT_pow2_zero_64(search_engine->results_list->width_in_bits));				// by default use what ever the constructor used
	#else
		if (sum_of_document_frequencies == 0)
			search_engine->set_accumulator_width(1);
		else
			search_engine->set_accumulator_width(ANT_max((long long)1, (long long)sqrt(64 * search_engine->document_count() / (double)sum_of_document_frequencies)));
	#endif
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
	Now ask the search engine to search
*/
#ifdef IMPACT_HEADER
	if (processing_strategy == ANT_ANT_param_block::TERM_AT_A_TIME)
		search_term_at_a_time(term_list, terms_in_query, ranking_function, expander_tf, stemmer);
	else if (processing_strategy == ANT_ANT_param_block::QUANTUM_AT_A_TIME)
		search_quantum_at_a_time(term_list, terms_in_query, ranking_function);
	else		// This is an error case, but do term at a time anyway
		search_term_at_a_time(term_list, terms_in_query, ranking_function, expander_tf, stemmer);

#else //if IMPACT_HRADER is not defined, can only use search_term_at_a_time
	search_term_at_a_time(term_list, terms_in_query, ranking_function, expander_tf, stemmer);
#endif

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
return process_NEXI_query(parse_NEXI_query(query), ranking_function);
}

/*
	ATIRE_API::PROCESS_TOPSIG_QUERY()
	---------------------------------
*/
long ATIRE_API::process_topsig_query(ANT_NEXI_term_ant *parse_tree)
{
ANT_string_pair as_string;
ANT_NEXI_term_ant *term_string;
ANT_NEXI_term_iterator term;
long terms_in_query, bit;
double *vector;

topsig_signature->rewind();
terms_in_query = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	{
	terms_in_query++;
	/*
		Take the search term (as an ANT_string_pair) and convert into a string
		If you want to know if the term is a + or - term then call term_string->get_sign() which will return 0 if it is not (or +ve or -ve if it is)
	*/
	string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer), true);

	if (search_engine->get_stemmer() != NULL)
		{
		search_engine->get_stemmer()->stem(token_buffer, stemmed_token_buffer);
		topsig_signature->add_term(topsig_globalstats, stemmed_token_buffer, 1, 1, topsig_globalstats->get_collection_length());
		}
	else
		topsig_signature->add_term(topsig_globalstats, token_buffer, 1, 1, topsig_globalstats->get_collection_length());
	}

/*
	Tell the search engine how many terms are in the query.  Although this is used in Language Models, it's unlikely
	to ever get used here.
*/
search_engine->results_list->set_term_count(terms_in_query);

/*
	Walk through the signature looking for +ve and -ve values as these are the
	dimenstions that are used in the query's signature.
*/
vector = topsig_signature->get_vector();
int set = 0;
for (bit = 0; bit < topsig_width; bit++)
	{
	if (vector[bit] != 0 && ANT_atosp(&as_string, bit) != NULL)
		{
		set++;
		if (vector[bit] > 0)
			search_engine->process_one_search_term(as_string.string(), topsig_positive_ranking_function);
		else
			search_engine->process_one_search_term(as_string.string(), topsig_negative_ranking_function);
		}
	}
printf("%d of %lld bits set in query\n", set, (long long)topsig_width);

return terms_in_query;
}

/*
	ATIRE_API::PROCESS_TOPSIG_QUERY()
	---------------------------------
*/
long ATIRE_API::process_topsig_query(char *query)
{
/*
	Initialise the shape of the 2D accumulators
*/
#ifdef TWO_D_ACCUMULATORS_POW2_WIDTH
	search_engine->set_accumulator_width(ANT_pow2_zero_64(search_engine->results_list->width_in_bits));				// by default use what ever the constructor used
#else
	search_engine->set_accumulator_width((long long)sqrt((double)search_engine->document_count()));		// by default use sqrt(N) as the width of the acumulator table
#endif

return process_topsig_query(parse_NEXI_query(query));
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

	string_pair_to_term(token_buffer, &root->term, sizeof(token_buffer), true);
	if (!ANT_islower(*token_buffer))		// We don't stem (or expand) numbers and tag names
		search_engine->process_one_search_term(token_buffer, ranking_function, 1, 1, 1, into);
	else
		{
		if (expander_tf != NULL)
			search_engine->process_one_thesaurus_search_term(expander_tf, stemmer, token_buffer, ranking_function, 1, 1, 1, into);
		else if (stemmer == NULL)
			search_engine->process_one_search_term(token_buffer, ranking_function, 1, 1, 1, into);
		else
			search_engine->process_one_stemmed_search_term(stemmer, token_buffer, ranking_function, 1, 1, 1, into);
		}

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
if (root == NULL)
	return;

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

ANT_search_engine_accumulator **accumulator_pointers;
long next_relevant_document;
ANT_heap<ANT_search_engine_accumulator *, ANT_search_engine_accumulator::compare> *heapk;

/*
	Parse the query and count the number of search terms.  If there's a parse error
	then don't do the query
*/
boolean_parser->set_thesaurus(expander_query);
boolean_parser->parse(parsed_query, query);
if (parsed_query->parse_error != ANT_query::ERROR_NONE)
	return 0;

/*
	Tell the search engine how many terms are in the query (because this is used in Language Models for ranking)
*/
search_engine->results_list->set_term_count(parsed_query->terms_in_query);

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
		answer = process_NEXI_query(parsed_query->NEXI_query, ranking_function);
		if (feedbacker == NULL)
			{
			delete [] parsed_query->NEXI_query;
			parsed_query->NEXI_query = NULL;
			}
		return answer;
		}
	}

/*
	Initialise the shape of the 2D accumulators
*/
#ifdef TWO_D_ACCUMULATORS_POW2_WIDTH
	search_engine->set_accumulator_width(ANT_pow2_zero_64(search_engine->results_list->width_in_bits));				// by default use what ever the constructor used
#else
	search_engine->set_accumulator_width((long long)sqrt((double)search_engine->document_count()));		// by default use sqrt(N) as the width of the acumulator table
#endif

/*
	Recurse over the tree
*/
valid_result_set = process_boolean_query(parsed_query->boolean_query, &terms_in_query);

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

/*
	Clean up and finish
*/
delete valid_result_set;

return terms_in_query;
}

/*
	ATIRE_API::QUERY_OBJECT_TURN_FEEDBACK_INTO_NEXI_QUERY()
	-------------------------------------------------------
*/
void ATIRE_API::query_object_turn_feedback_into_NEXI_query(void)
{
ANT_NEXI_term_ant *new_query, *term;
long current_feedback;

new_query = new ANT_NEXI_term_ant [parsed_query->feedback_terms_in_query];

/*
	Add the feedback terms
*/
for (current_feedback = 0; current_feedback < parsed_query->feedback_terms_in_query; current_feedback++)
	{
	term = new_query + current_feedback;
	term->next = term + 1;
	term->parent_path = NULL;
	term->path.start = NULL;
	term->sign = 0;
	term->tf_weight = 1;
	term->rsv_weight = parsed_query->feedback_terms[current_feedback]->kl_score;
	term->term = parsed_query->feedback_terms[current_feedback]->string;
	term->query_frequency = 1;
	}

parsed_query->terms_in_query = parsed_query->feedback_terms_in_query;
new_query[parsed_query->terms_in_query - 1].next = NULL;

if (parsed_query->type & QUERY_BOOLEAN)
	delete [] parsed_query->NEXI_query;

parsed_query->NEXI_query = new_query;
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
	term->tf_weight = 1;
	term->rsv_weight = parsed_query->feedback_terms[current_feedback]->kl_score;
	term->term = parsed_query->feedback_terms[current_feedback]->string;
	term->query_frequency = 1;
	}
parsed_query->terms_in_query = parsed_query->terms_in_query + parsed_query->feedback_terms_in_query;
new_query[parsed_query->terms_in_query - 1].next = NULL;

#ifdef NEVER
	/*
		This code prints out the expanded query
	*/
	{
	long which;
	for (which = 0; new_query[which].next != NULL; which++)
		printf("%*.*s ", (int)new_query[which].term.length(), (int)new_query[which].term.length(), new_query[which].term.string());
	printf("%*.*s\n", (int)new_query[which].term.length(), (int)new_query[which].term.length(), new_query[which].term.string());
	}
#endif

if (parsed_query->type & QUERY_BOOLEAN)
	delete [] parsed_query->NEXI_query;

parsed_query->NEXI_query = new_query;
}

/*
	ATIRE_API::FEEDBACK_INTERPOLATED()
	----------------------------------
	Puurula's ALTA algorithm
*/
void ATIRE_API::feedback_interpolated(long long top_k)
{
ANT_search_engine_memory_index *memory_index;
ANT_NEXI_term_iterator term_iterator;
ANT_search_engine_result_id_iterator iterator;
ANT_NEXI_term_ant *term_string;
double normalizer, term_normaliser, document_score, document_term_score;
ANT_search_engine_btree_leaf term_details;
long long id, docid;
long long term;
double terms_in_the_query;
unsigned short term_frequency;
long long document_frequency, collection_frequency;
double sum_of_query_frequencies;
size_t documents_to_examine;
char raw_token_buffer[MAX_TERM_LENGTH];

/*
	Build an index from the top feedback_documents documents.  Note that if there's a stemmer being used then the
	resultant search_engine_memory_index will behave as a stemmed index.
*/
documents_to_examine = ANT_min((size_t)feedback_documents, (size_t)search_engine->results_list->results_list_length);
memory_index = rerank(documents_to_examine);

/*
	Compute the term-specific weighting component normlizer
*/
normalizer = 0;
terms_in_the_query = 0;
for (term_string = (ANT_NEXI_term_ant *)term_iterator.first(parsed_query->NEXI_query), term = 0; term_string != NULL; term_string = (ANT_NEXI_term_ant *)term_iterator.next(), term++)
	{
	terms_in_the_query += term_string->query_frequency;
	/*
		get the search term and its stats in the top few documents
	*/
	if (stemmer == NULL || term_string->get_term()->string_length <= 3)
		string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer), true);
	else
		{
		string_pair_to_term(raw_token_buffer, term_string->get_term(), sizeof(token_buffer), true);
		stemmer->stem(raw_token_buffer, token_buffer);
		}

	if (memory_index->process_one_term(token_buffer, &term_details) == NULL)
		continue;		// term not found in the top document

	/*
		Turn the poistings list into an array[index] where index is the document number in the range 1..documents_to_examine.  The
		result is in search_engine->stem_buffer
	*/
	memset(memory_index->stem_buffer, 0, documents_to_examine * sizeof(*memory_index->stem_buffer));
	memory_index->place_into_internal_buffers(&term_details);

	term_normaliser = 0;
	for (id = 0; id < documents_to_examine; id++)
		{
		docid = search_engine->results_list->accumulator_pointers[id] - search_engine->results_list->accumulator;

		term_frequency = memory_index->stem_buffer[id];			// the number of times the term occurs in the document
		collection_frequency = term_details.global_collection_frequency;
		document_frequency = term_details.global_document_frequency;

		document_term_score = ranking_function->score_one_document((ANT_compressable_integer)docid, (ANT_compressable_integer)search_engine->document_lengths[docid], term_frequency, collection_frequency, document_frequency, term_string->query_frequency, search_engine->results_list->get_term_count());
		document_score = search_engine->results_list->accumulator[docid].get_rsv();
		term_normaliser = ANT_logsum(term_normaliser, document_term_score + document_score);
		}
	normalizer = ANT_logsum(normalizer, term_normaliser);
	}

/*
	Build the term specific weight
*/
sum_of_query_frequencies = 0;
for (term_string = (ANT_NEXI_term_ant *)term_iterator.first(parsed_query->NEXI_query), term = 0; term_string != NULL; term_string = (ANT_NEXI_term_ant *)term_iterator.next(), term++)
	{
	/*
		get the search term and its stats in the top few documents
	*/
	if (stemmer == NULL || term_string->get_term()->string_length <= 3)
		string_pair_to_term(token_buffer, term_string->get_term(), sizeof(token_buffer), true);
	else
		{
		string_pair_to_term(raw_token_buffer, term_string->get_term(), sizeof(token_buffer), true);
		stemmer->stem(raw_token_buffer, token_buffer);
		}

	if (memory_index->process_one_term(token_buffer, &term_details) == NULL)
		continue;		// term not found in the top document

	/*
		Turn the poistings list into an array[index] where index is the document number in the range 1..documents_to_examine.  The
		result is in search_engine->stem_buffer
	*/
	memset(memory_index->stem_buffer, 0, documents_to_examine * sizeof(*memory_index->stem_buffer));
	memory_index->place_into_internal_buffers(&term_details);

	term_normaliser = 0;

	for (id = 0; id < documents_to_examine; id++)
		{
		docid = search_engine->results_list->accumulator_pointers[id] - search_engine->results_list->accumulator;

		term_frequency = memory_index->stem_buffer[id];			// the number of times the term occurs in the document
		collection_frequency = term_details.global_collection_frequency;
		document_frequency = term_details.global_document_frequency;

		document_term_score = ranking_function->score_one_document((ANT_compressable_integer)docid, (ANT_compressable_integer)search_engine->document_lengths[docid], term_frequency, collection_frequency, document_frequency, term_string->query_frequency, search_engine->results_list->get_term_count());
		document_score = search_engine->results_list->accumulator[docid].get_rsv();
		term_normaliser = ANT_logsum(term_normaliser, document_term_score + document_score - normalizer);
		}
	term_string->query_frequency = ((1 - feedback_lambda) * term_string->query_frequency / terms_in_the_query) + (feedback_lambda * exp(term_normaliser));
	sum_of_query_frequencies += term_string->query_frequency;
	}

/*
	Re-do the query with the new weights
*/
/*
	Initialise
*/
search_engine->init_accumulators(top_k);

/*
	Search
*/
process_NEXI_query(parsed_query->NEXI_query, feedback_ranking_function == NULL ? ranking_function : feedback_ranking_function, sum_of_query_frequencies);

/*
	Rank
*/
search_engine->sort_results_list(top_k, &hits);

/*
	Clean up
*/
delete memory_index;
}

/*
	ATIRE_API::FEEDBACK()
	---------------------
*/
void ATIRE_API::feedback(long long top_k)
{
if (feedback_mode == FEEDBACK_INTERPOLATED)
	{
	feedback_interpolated(top_k);
	return;
	}
/*
	Get the feedback terms
*/
parsed_query->feedback_terms = feedbacker->feedback(search_engine->results_list, parsed_query, feedback_documents, feedback_terms, &parsed_query->feedback_terms_in_query);

#ifdef NEVER
	/*
		Print out the feedback terms
	*/%
	printf("\nFEEDBACK TERMS:");
	for (ANT_memory_index_one_node **current = parsed_query->feedback_terms; *current != NULL; current++)
		printf("%*.*s ", (*current)->string.length(), (*current)->string.length(), (*current)->string.start);
	puts("");
#endif

/*
	If we have and feedback terms then do a NEXI query.  Note that if the documents are *not*
	in the index then there will be no feedback terms an so this will not happen
*/
if (parsed_query->feedback_terms_in_query != 0)
	{
	/*
		Initialise
	*/
	search_engine->init_accumulators(top_k);

	/*
		Generate query, search, and clean up
	*/
	query_object_with_feedback_to_NEXI_query();

	process_NEXI_query(parsed_query->NEXI_query, feedback_ranking_function == NULL ? ranking_function : feedback_ranking_function);
	delete [] parsed_query->feedback_terms;
	delete [] parsed_query->NEXI_query;

	/*
		Rank
	*/
	search_engine->sort_results_list(top_k, &hits);
	}
}

/*
	ATIRE_API::TOPSIG_FEEDBACK()
	----------------------------
*/
void ATIRE_API::topsig_feedback(long long top_k)
{
ANT_string_pair as_string;
double *feedback_vector;
long bit;

/*
	Get the feedback vector
*/
feedback_vector = (dynamic_cast<ANT_relevance_feedback_topsig *>(feedbacker))->feedback(search_engine->results_list, feedback_documents);

/*
	Now pass that to the TopSig search engine
*/
if (feedback_vector != NULL)
	{
	/*
		Initialise
	*/
	search_engine->init_accumulators(top_k);

	/*
		Walk through the signature looking for +ve and -ve values as these are the
		dimenstions that are used in the query's signature.
	*/
	int set = 0;
	for (bit = 0; bit < topsig_width; bit++)
		if (feedback_vector[bit] != 0 && ANT_atosp(&as_string, bit) != NULL)
			{
			set++;
			if (feedback_vector[bit] > 0)
				search_engine->process_one_search_term(as_string.string(), topsig_positive_ranking_function);
			else
				search_engine->process_one_search_term(as_string.string(), topsig_negative_ranking_function);
			}
//	printf("%d of %lld bits set in query\n", set, (long long)topsig_width);

	/*
		Rank
	*/
	search_engine->sort_results_list(top_k, &hits);
	}
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
if (processing_strategy == ANT_ANT_param_block::QUANTUM_AT_A_TIME)
	search_engine->init_accumulators(top_k == LLONG_MAX ? top_k : top_k + 1);
else
	search_engine->init_accumulators(top_k);

/*
	Parse and do the query
*/
if (query_type & QUERY_NEXI)
	parsed_query->terms_in_query = process_NEXI_query(query);
else if (query_type & QUERY_BOOLEAN)
	parsed_query->terms_in_query = process_boolean_query(query);
else if (query_type & QUERY_TOPSIG)
	parsed_query->terms_in_query = process_topsig_query(query);
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
	if (query_type & (QUERY_NEXI | QUERY_BOOLEAN))
		feedback(top_k);
	else if (query_type & QUERY_TOPSIG)
		topsig_feedback(top_k);

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
ANT_search_engine_memory_index *ATIRE_API::rerank(long long top_k)
{
ANT_memory_index *indexer;
ANT_search_engine_memory_index *in_memory_index;
long long current, top_n;
char *document_buffer;
long long documents_to_examine = top_k;		// load and parse and reindex this many documents;
long long docid;
unsigned long current_document_length;
ANT_directory_iterator_object object;
ANT_readability_factory *readability;
ANT_parser *parser;
ANT_memory *memory;
ANT_stem_stemmer stemming_function(stemmer);

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
	document_indexer->index_document(indexer, stemmer == NULL ? NULL : &stemming_function, TRUE, readability, current + 1, object.file);		// indexing counts from 1 (searching counts from 0)
	}

delete [] document_buffer;
delete readability;
delete parser;

/*
	turn the index into a search engine.
*/
indexer->allocate_decompress_buffer();
memory = new ANT_memory;
in_memory_index = new ANT_search_engine_memory_index(indexer, memory);
in_memory_index->open();

return in_memory_index;
}

#ifdef FILENAME_INDEX
	/*
		ATIRE_API::GET_DOCUMENT_FILENAME()
		----------------------------------
	*/
	char *ATIRE_API::get_document_filename(char *filename, long long internal_document_id)
	{
	return search_engine->get_document_filename(filename, internal_document_id);
	}

#else
	/*
		ATIRE_API::GENERATE_RESULTS_LIST()
		----------------------------------
	*/
	char **ATIRE_API::generate_results_list(void)
	{
	search_engine->generate_results_list(filename_list, answer_list, hits);
	return answer_list;
	}
#endif

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
	ATIRE_API::SET_PROCESSING_STRATEGY()
	------------------------------------
*/
void ATIRE_API::set_processing_strategy(long new_strategy, uint8_t early_termination_strategy)
{
long long i;
processing_strategy = new_strategy;
early_termination = early_termination_strategy;

if (processing_strategy == ANT_ANT_param_block::QUANTUM_AT_A_TIME)
	{
	memory->realign();
	impact_header_buffers = (ANT_impact_header **)memory->malloc(MAX_ALLOWED_TERMS_IN_QUERY * sizeof(*impact_header_buffers));
	for (i = 0; i < MAX_ALLOWED_TERMS_IN_QUERY; i++)
		{
		impact_header_buffers[i] = (ANT_impact_header *)malloc(sizeof(**impact_header_buffers));
		impact_header_buffers[i]->header_buffer = (ANT_compressable_integer *)malloc(sizeof(*impact_header_buffers[0]->header_buffer) * ANT_impact_header::NUM_OF_QUANTUMS * 3 + ANT_COMPRESSION_FACTORY_END_PADDING);
		}

	memory->realign();
	max_quantums = (ANT_max_quantum *)memory->malloc(MAX_ALLOWED_TERMS_IN_QUERY * sizeof(*max_quantums));
	memory->realign();
	max_quantums_pointers = (ANT_max_quantum **)memory->malloc(MAX_ALLOWED_TERMS_IN_QUERY * sizeof(**max_quantums_pointers));
	memory->realign();
	quantum_heap = new ANT_heap<ANT_max_quantum *, ANT_max_quantum::compare>(*max_quantums_pointers, MAX_ALLOWED_TERMS_IN_QUERY);
	heap_items = 0;

	memory->realign();
	raw_postings_buffer = (unsigned char *)memory->malloc(search_engine->get_postings_buffer_length());
	memory->realign();
	one_decompressed_quantum = (ANT_compressable_integer *)memory->malloc(ANT_impact_header::NUM_OF_QUANTUMS * sizeof(*one_decompressed_quantum));

	// reset the early termination strategy if the index is TF impacted
	if (early_termination != ANT_ANT_param_block::QUANTUM_STOP_NONE && !search_engine->get_is_quantized())
		{
		early_termination = ANT_ANT_param_block::QUANTUM_STOP_NONE;
		printf("WARNING: the index is not quantized, the early termination for quantum-at-a-time is reset to none.\n");
		}
	}
}


/*
	ATIRE_API::GET_DOCUMENT_COUNT()
	-------------------------------
*/
long long ATIRE_API::get_document_count(void)
{
return search_engine->document_count();
}

/*
	ATIRE_API::GET_TERM_COUNT()
	---------------------------
*/
long long ATIRE_API::get_term_count(void)
{
return search_engine->term_count();
}

/*
	ATIRE_API::GET_UNIQUE_TERM_COUNT()
	----------------------------------
*/
long long ATIRE_API::get_unique_term_count(void)
{
return search_engine->get_unique_term_count();
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

/*
	ATIRE_API::EXTRACT_QUERY_TERMS()
	--------------------------------
*/
char *ATIRE_API::extract_query_terms(char *document, long terms)
{
long terms_found;
ANT_memory_index_one_node **feedback_terms;
std::ostringstream query;

if (more_like_term_chooser == NULL)
	{
	ANT_relevance_feedback_factory factory;
	more_like_term_chooser = factory.get_feedbacker(search_engine, ANT_relevance_feedback_factory::BLIND_KL);
	}

feedback_terms = more_like_term_chooser->feedback(document, terms, &terms_found);

for (ANT_memory_index_one_node **current = feedback_terms; *current != NULL; current++)
	query.write((*current)->string.start, (*current)->string.length()) << ' ';

delete [] feedback_terms;

return strnew(query.str().c_str());
}

