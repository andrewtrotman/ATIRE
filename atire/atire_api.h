/*
	ATIRE_API.H
	-----------
*/
#ifndef ATIRE_API_H_
#define ATIRE_API_H_

#include <limits.h>
#include "heap.h"
#include "max_quantum.h"
#include "compress.h"

/*
     the reason for removing btree_iterator.h here is that when the atire api headers can be installed in a system path, and they should be kept minimum.
     We need only a few api headers disclosed to users, and btree_iterator.h really should not be one of them (api heades). If this header is kept here, compiler will complain not being able to locate this header file.
     It is the same for other headers, api should provide only key access to the main search engine functions, only necessary other supporting functions

     It may need a bit re-design like we better not to include too much headers in the key api header
     Any other better solutions?
 */
#define MAX_TERM_LENGTH (20 * 1024)

class ANT_NEXI_ant;
class ANT_NEXI_term_ant;
class ANT_search_engine;
class ANT_stemmer;
class ANT_query_boolean;
class ANT_query;
class ANT_ranking_function;
class ANT_memory;
class ANT_memory_index;
class ANT_memory_index_one;
class ANT_relevant_document;
class ANT_search_engine_forum;
class ANT_assessment_factory;
class ANT_bitstring;
class ANT_query_parse_tree;
class ANT_string_pair;
class ANT_relevance_feedback;
class ANT_pregen;
class ANT_index_document;
class ANT_index_document_topsig;
class ANT_index_document_topsig_signature;
class ANT_thesaurus;
class ANT_evaluator;
class ANT_ANT_param_block;

/*
	class ATIRE_API
	---------------
*/
class ATIRE_API
{
public:
	enum { INDEX_IN_FILE = 0, INDEX_IN_MEMORY = 1, READABILITY_SEARCH_ENGINE = 2 } ;
	enum { QUERY_NEXI = 1, QUERY_BOOLEAN = 2, QUERY_TOPSIG = 4, QUERY_FEEDBACK = 8, QUERY_EXPANSION_INPLACE_WORDNET = 16, QUERY_EXPANSION_WORDNET = 32 } ;
	static const int MAX_ALLOWED_TERMS_IN_QUERY = 1024;

private:
	char token_buffer[MAX_TERM_LENGTH];			// used to convert parsed string_pairs into C char * strings.
	char stemmed_token_buffer[MAX_TERM_LENGTH];	// used (in TopSig) to get a stemmed verson of the search term.
	ANT_memory *memory;						// ATIRE memory allocation scheme

	ANT_NEXI_ant *NEXI_parser;				// INEX CO / CAS queries
	ANT_query_boolean *boolean_parser;		// full boolean
	long segmentation;						// Chinese segmentation algorithm
	ANT_query *parsed_query;				// the parsed query
	ANT_search_engine *search_engine;		// the search engine itself
	ANT_ranking_function *ranking_function;	// the ranking function to use (default is the perameterless Divergence From Randomness)
	ANT_stemmer *stemmer;					// stemming function to use
	ANT_relevance_feedback *feedbacker;		// relevance feedback algorithm to use (NULL = none)
	long feedback_documents;				// documents to analyse in relevance feedback
	long feedback_terms;					// terms (extracted from top documents) to use in relevance feedback
	long query_type_is_all_terms;			// use the DISJUNCTIVE ranker but only find documents containing all of the search terms (CONJUNCTIVE)
	long long hits;							// how many documents were found at the last query
	long long sort_top_k;					// ranking is only accurate to this position in the results list

	char **document_list;					// list (in order) of the external IDs of the documents in the collection
	char **filename_list;					// the same list, but assuming filenames (parsed for INEX)
	char **answer_list;						//
	long long documents_in_id_list;			// the length of the above two lists (the number of docs in the collection)
	char *mem1, *mem2;						// arrays of memory holding the above;

	ANT_assessment_factory *assessment_factory;		// the machinery to read different formats of assessments (INEX and TREC)
	ANT_relevant_document *assessments;		// assessments for measuring percision (at TREC and INEX)
	long long number_of_assessments;		// length of the assessments array
	ANT_evaluator *evaluator;
	ANT_search_engine_forum *forum_writer;	// the object that writes the results list in the INEX or TREC format
	long forum_results_list_length;			// maximum length of a results list for the evaluation form (INEX or TREC)

	ANT_pregen *pregens;					// pre-loaded pregens
	long pregen_count;						// number of pregens which have been loaded

	ANT_index_document *document_indexer;	// the file inverter

	long topsig_width;											// width of the bitstring (dimensions to the vector)
	double topsig_density;										// the proportion of bits that should be set in the TopSig signature
	ANT_index_document_topsig *topsig_globalstats;				// term statistics if topsig is being used
	ANT_index_document_topsig_signature *topsig_signature;		// the query signature
	ANT_ranking_function *topsig_positive_ranking_function;		// the ranking function to use for +ve values in the query vector
	ANT_ranking_function *topsig_negative_ranking_function;		// the ranking function to use for -ve values in the query vector

	ANT_thesaurus *expander_tf;									// TF expansion::this is the tool to use to find synonyms of the query terms
	ANT_thesaurus *expander_query;								// Query expansion::this is the tool to use to find synonyms of the query terms

	long processing_strategy;									// term or quantum at a time (term by default)

	// for quantum-at-a-time
	ANT_impact_header **impact_header_buffers;
	long long number_of_impact_headers;
	unsigned char *raw_postings_buffer;
	ANT_compressable_integer *one_decompressed_quantum;
	ANT_max_quantum *max_quantums;
	ANT_max_quantum **max_quantums_pointers;
	ANT_heap<ANT_max_quantum *, ANT_max_quantum::compare> *quantum_heap;
	long long heap_items;
	long long max_remaining_quantum;
	uint8_t early_termination;
	long long processed_postings;

protected:
	char **read_docid_list(char * doclist_filename, long long *documents_in_id_list, char ***filename_list, char **mem1, char **mem2);
	static char *max(char *a, char *b, char *c);
	long process_NEXI_query(char *query);
	ANT_bitstring *process_boolean_query(ANT_query_parse_tree *root, long *leaves);
	long process_topsig_query(ANT_NEXI_term_ant *parse_tree);
	void boolean_to_NEXI(ANT_NEXI_term_ant *into, ANT_query_parse_tree *root, long *leaves);
	long process_NEXI_query(ANT_NEXI_term_ant *parse_tree);
	long process_boolean_query(char *query);
	long process_topsig_query(char *query);
	char *string_pair_to_term(char *destination, ANT_string_pair *source, size_t destination_length, long case_fold = 0);
	void query_object_with_feedback_to_NEXI_query(void);
	void feedback(long long top_k);
	void topsig_feedback(long long top_k);
	void search_term_at_a_time(ANT_NEXI_term_ant **term_list, long long terms_in_query, ANT_ranking_function *ranking_function, ANT_thesaurus *expander_tf, ANT_stemmer *stemmer);
#ifdef IMPACT_HEADER
	void search_quantum_at_a_time(ANT_NEXI_term_ant **term_list, long long terms_in_query, ANT_ranking_function *ranking_function);
#endif

public:
	ATIRE_API();
	virtual ~ATIRE_API();

	/*
		What version are we?
	*/
	char *version(long *version_number = 0);

	/*
		Load all the necessary stuff for the search engine to start up
		This assumes we are in same directory as the index
	*/
	long open(long type, char *index_filename, char *doclist_filename, long quantize, long long quantization_bits);		// see the enum above for possible types (ORed together)

	ANT_search_engine *get_search_engine(void) { return search_engine; }

	/*
		Load an assessment file (for INEX or TREC)
	*/
	long load_assessments(char *assessments_filename, ANT_evaluator *evaluator);

	/*
	    Load a pregenerated ranking
	 */
	long load_pregen(const char *pregen_filename);

	/*
		Load TopSig globalstats file
	*/
	long load_topsig(long width, double density, char *global_stats_file);

	/*
		Set the chinese segmentation algorithm
	*/
	void set_segmentation(long segmentation) { this->segmentation = segmentation; }

	/*
		Parse a NEXI query
	*/
	ANT_NEXI_term_ant *parse_NEXI_query(char *query);

	/*
		Set the ranking function
		for BM25: k1 = p1, b = k2
		for LMD:  u = p1
		for LMJM: l = p1
	*/
	long set_ranking_function(long long function, long quantization, long long quantization_bits, double p1, double p2);
	long set_ranking_function_pregen(const char *fieldname, double p1);

	ANT_pregen *get_pregen();

	/*
		Set the stemming function
	*/
	long set_stemmer(long which_stemmer, long stemmer_similarity, double threshold);

	/*
		Set the thesaurus to use for inplace TF synonym expansion
	*/
	long set_inplace_query_expansion(ANT_thesaurus *expander);

	/*
		Set the thesaurus to use for classic query expansion
	*/
	long set_query_expansion(ANT_thesaurus *expander);

	/*
		Set the relevance feedback mechanism
	*/
	long set_feedbacker(long feedbacker, long documents, long terms);

	/*
		Set the static pruning point.  At most sttic_prune_point postings will be read from disk and processedS
	*/
	long long set_trim_postings_k(long long static_prune_point);

	/*
		Set term or quantum at a time processing of the postings lists
	*/
	void set_processing_strategy(long new_strategy, uint8_t early_termination_strategy);

	/*
		Given the query, do the seach, rank, and return the number of hits
		query_type is either QUERY_NEXI or QUERY_BOOLEAN
	*/
	long long search(char *query, long long top_k = LLONG_MAX, long query_type = QUERY_NEXI);

	/*
		Call the re-ranker.  This takes the top few results and re-orders based on
		analysis of the result set.
	*/
	void rerank(void);

	/*
		Turn the numeric internal IDs into a list of external string IDs (post search)
	*/
	char **generate_results_list(void);

	/*
		Given a positing in the results list return the internal search engine docid and its relevance
	*/
	long long get_relevant_document_details(long long result, long long *docid, double *relevance);

	/*
		Configure TREC or INEX output format
	*/
	long set_forum(long type, char *output_filename, char *participant_id, char *run_name, long forum_results_list_length);

	/*
		Write the results out in INEX or TREC format (as specified by set_form)
	*/
	void write_to_forum_file(long topic_id);

	/*
		Return the number of documents in the index (for idf computation)
	*/
	long long get_document_count(void);

	/*
		Return the number of terms in the index (for icf computation)
	*/
	long long get_term_count(void);

	/*
		Return the number of unique terms in the index (for merging)
	*/
	long long get_unique_term_count(void);

	/*
		Get the length of the longest document in the repository
		useful so that you can allocate a buffer for get_document.
	*/
	long get_longest_document_length(void);

	/*
		Load a document from the repository (if there is one)
	*/
	char *get_document(char *buffer, unsigned long *length, long long id);

	/*
		Rendering of statistics to do with the last query
	*/
	void stats_text_render(void);

	/*
		Rendering of statistics to do with all queries so far since the search engine started
	*/
	void stats_all_text_render(void);
} ;

#endif /* ATIRE_API_H_ */
