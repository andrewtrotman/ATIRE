/*
	SEARCH_ENGINE.H
	---------------
*/
#ifndef SEARCH_ENGINE_H_
#define SEARCH_ENGINE_H_

#include "fundamental_types.h"
#include "compression_factory.h"
#include "compression_text_factory.h"
#include "ranking_function.h"
#include "btree_iterator.h"
#include "search_engine_accumulator.h"
#include "impact_header.h"
#include "version.h"
#include "quantum.h"

class ANT_memory;
class ANT_file;
class ANT_search_engine_btree_node;
class ANT_search_engine_btree_leaf;
class ANT_search_engine_accumulator;
class ANT_search_engine_result;
class ANT_stats_search_engine;
class ANT_stem;
class ANT_stemmer;
class ANT_ranking_function;
class ANT_thesaurus;

/*
	class ANT_SEARCH_ENGINE
	-----------------------
*/
class ANT_search_engine
{
friend class ANT_btree_iterator;
friend class ANT_mean_average_precision;
friend class ANT_search_engine_result_iterator;
friend class ANT_relevance_feedback;
friend class ATIRE_API;

private:
	ANT_stats_search_engine *stats;
	ANT_stats_search_engine *stats_for_all_queries;
	ANT_file *index;
	ANT_search_engine_btree_node *btree_root;
	long btree_nodes;
	long long max_header_block_size;
	long string_length_of_longest_term;
	long long highest_df;
	const char *index_filename;
	long memory_model;

	/*
		If the index contains the documents then we need to deserialise the positions and store them
	*/
	ANT_compression_text_factory document_factory;
	long long *document_offsets;
	long long document_longest_compressed;
	char *document_decompress_buffer;
	long document_longest_raw_length;		// length of the longest document in the collection (once it has been decompressed).

	/*
		If the index is stemmed then we need a stemmer to stem the query terms
	*/
	ANT_stem *stemmer;
	char stemmed_term[MAX_TERM_LENGTH];

protected:
#ifdef IMPACT_HEADER
	ANT_impact_header impact_header;
#endif
	ANT_compressable_integer *decompress_buffer;
	ANT_compression_factory factory;
	ANT_memory *memory;
	long long documents;
	long postings_buffer_length;
	unsigned char *btree_leaf_buffer, *postings_buffer;
	ANT_weighted_tf *stem_buffer;
	ANT_compressable_integer *document_lengths;
	double mean_document_length;
	long long stem_buffer_length_in_bytes;
	long long trim_postings_k;				// the query-based static pruning point (as set in the query)
	long long global_trim_postings_k;		// the index-based static pruning point (as set by the indexer)
	long long collection_length_in_terms;
	long unique_terms;
	long long is_quantized;					// true if the index is quantized, false if the index is TF values.

public:
	ANT_search_engine_result *results_list;

private:
	ANT_search_engine_btree_leaf *get_leaf(unsigned char *leaf, long term_in_leaf, ANT_search_engine_btree_leaf *term_details);
	void initialise(ANT_memory *memory);
	long long get_btree_leaf_position(char *term, long long *length, long *exact_match, long *btree_root_node);
#ifdef USE_FLOATED_TF
	long long place_into_internal_buffers(ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf term_frequency_weight = 1);
#else
	long long place_into_internal_buffers(ANT_search_engine_btree_leaf *term_details);
#endif
	long long stem_into_internal_buffers(ANT_stemmer *stemmer, char *base_term);

public:
	ANT_search_engine(ANT_memory *memory, long memory_model = 0);
	virtual ~ANT_search_engine();

	virtual int open(const char *filename = "index.aspt");
	void set_accumulator_width(long long width);
	void init_accumulators(long long top_k);
	ANT_search_engine_btree_leaf *get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details);
	unsigned char *get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination);
#ifdef IMPACT_HEADER
	unsigned char *get_one_quantum(ANT_search_engine_btree_leaf *term_details, ANT_impact_header *the_impact_header, ANT_quantum *the_quantum, unsigned char *destination);
	unsigned char *get_impact_header(ANT_search_engine_btree_leaf *term_details, unsigned char *destination);
#endif
	long long get_is_quantized() { return is_quantized; }
	long long get_postings_buffer_length() { return postings_buffer_length; }

	virtual ANT_search_engine_btree_leaf *process_one_term(char *term, ANT_search_engine_btree_leaf *term_details);
	virtual ANT_search_engine_btree_leaf *get_collection_frequency(char *base_term, ANT_stem *stem_maker, ANT_search_engine_btree_leaf *stemmed_term_details);
#ifdef IMPACT_HEADER
	void *read_and_decompress_for_one_quantum(ANT_search_engine_btree_leaf *term_details,
						unsigned char *raw_postings_buffer,
						ANT_impact_header *the_impact_header,
						ANT_quantum *the_quantum,
						ANT_compressable_integer *the_decompressed_buffer);
	void *read_and_decompress_for_one_impact_header(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_impact_header *the_impact_header);
	void *read_and_decompress_for_one_term(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_impact_header *the_impact_header, ANT_compressable_integer *the_decompressed_buffer);
#else
	void *read_and_decompress_for_one_term(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_compressable_integer *the_decompressed_buffer);
#endif
	virtual void process_one_term_detail(ANT_search_engine_btree_leaf *term_details, ANT_ranking_function *ranking_function, ANT_bitstring *bitstring = NULL);
	virtual void process_one_search_term(char *term, ANT_ranking_function *ranking_function, ANT_bitstring *bitstring = NULL);
	virtual void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function, ANT_bitstring *bitstring = NULL);
	virtual void process_one_thesaurus_search_term(ANT_thesaurus *expander, ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function, ANT_bitstring *bitstring = NULL);

	ANT_search_engine_accumulator **sort_results_list(long long accurrate_rank_point, long long *hits);
	char **generate_results_list(char **document_id_list, char **sorted_id_list, long long top_k);
	long long document_count(void) { return documents; }
	long long term_count(void) { return collection_length_in_terms; }
	ANT_compressable_integer *get_document_lengths(double *mean) { *mean = mean_document_length; return document_lengths; }
#ifdef IMPACT_HEADER
	ANT_impact_header *get_impact_header(void) { return &impact_header; }
#endif

	ANT_compressable_integer *get_decompress_buffer(void) { return decompress_buffer; }
	unsigned char *get_postings_buffer(void) { return postings_buffer; }
	ANT_stats_search_engine *get_stats(void) { return stats; }
	void stats_initialise(void);
	void stats_text_render(void);
	void stats_all_text_render(void);
	void stats_add(void);
	long long set_trim_postings_k(long long what) { long long old = trim_postings_k; trim_postings_k = what; return old; }
	long long get_trim_postings_k(void) { return trim_postings_k; }
	long long get_global_trim_postings_k(void) { return global_trim_postings_k; }
	long long get_collection_length(void) { return collection_length_in_terms; }
	long long get_unique_term_count(void) { return unique_terms; }

	ANT_compressable_integer *get_decompressed_postings(char *term, ANT_search_engine_btree_leaf *term_details);

	long long boolean_results_list(long terms_in_query);
	long long get_variable(char *name);
	long long quantized(void) { return is_quantized; }					// true if the index is quantized, false if the index is TF values.

	ANT_stem *get_stemmer(void) { return stemmer; }
	/*
		Methods related to the retrieval of documents from the document repository.
	*/
	long get_longest_document_length(void) { return document_longest_raw_length; }
	char *get_compressed_document(char *destination, unsigned long *destination_length, long long id); // id is the document number
	char *get_document(char *destination, unsigned long *destination_length, long long id); // id is the document number
	long long get_document(char **destination, unsigned long **destination_length, long long from) { return get_documents(destination, destination_length, from, from + 1); }
	long long get_documents(char **destination, unsigned long **destination_length, long long from, long long to); // from and to are relative to the results list.

	char **get_document_filenames(char *buffer, unsigned long *buf_length);
};

#endif  /* SEARCH_ENGINE_H_ */
