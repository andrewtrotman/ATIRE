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

/*
	class ANT_SEARCH_ENGINE
	-----------------------
*/
class ANT_search_engine
{
friend class ANT_btree_iterator;
friend class ANT_mean_average_precision;
friend class ANT_search_engine_result_iterator;

private:
	ANT_stats_search_engine *stats;
	ANT_stats_search_engine *stats_for_all_queries;
	ANT_memory *memory;
	ANT_file *index;
	ANT_search_engine_btree_node *btree_root;
	long btree_nodes;
	long long max_header_block_size;
	long string_length_of_longest_term;
	long long highest_df;
	const char *index_filename;

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
	ANT_compressable_integer *decompress_buffer;
	ANT_compression_factory factory;
	long long documents;
	unsigned char *btree_leaf_buffer, *postings_buffer;
	ANT_weighted_tf *stem_buffer;
	ANT_compressable_integer *document_lengths;
	double mean_document_length;
	long long stem_buffer_length_in_bytes;
	long long trim_postings_k;
	long long collection_length_in_terms;

public:
	ANT_search_engine_result *results_list;

private:
	ANT_search_engine_btree_leaf *get_leaf(unsigned char *leaf, long term_in_leaf, ANT_search_engine_btree_leaf *term_details);
	void initialise(ANT_memory *memory);

public:
	ANT_search_engine(ANT_memory *memory, long memory_model = 0, const char *index_filename = "index.aspt");
	virtual ~ANT_search_engine();

#ifdef TOP_K_SEARCH
	void init_accumulators(long long top_k);
#else
	void init_accumulators(void);
#endif
	long long get_btree_leaf_position(char *term, long long *length, long *exact_match, long *btree_root_node);
	ANT_search_engine_btree_leaf *get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details);
	unsigned char *get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination);

	virtual ANT_search_engine_btree_leaf *process_one_term(char *term, ANT_search_engine_btree_leaf *term_details);
	virtual void process_one_term_detail(ANT_search_engine_btree_leaf *term_details, ANT_ranking_function *ranking_function);
	virtual void process_one_search_term(char *term, ANT_ranking_function *ranking_function);
	virtual void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function);
	ANT_search_engine_accumulator **sort_results_list(long long accurrate_rank_point, long long *hits);
	char **generate_results_list(char **document_id_list, char **sorted_id_list, long long top_k);
	long long document_count(void) { return documents; }
	ANT_compressable_integer *get_document_lengths(double *mean) { *mean = mean_document_length; return document_lengths; }
	ANT_compressable_integer *get_decompress_buffer(void) { return decompress_buffer; }
	ANT_stats_search_engine *get_stats(void) { return stats; }
	void stats_initialise(void);
	void stats_text_render(void);
	void stats_all_text_render(void);
	void stats_add(void);
	void set_trim_postings_k(long long what) { trim_postings_k = what; }
	long long get_collection_length(void) { return collection_length_in_terms; }

	ANT_compressable_integer *get_decompressed_postings(char *term, ANT_search_engine_btree_leaf *term_details);

	long long boolean_results_list(long terms_in_query);

	long long get_variable(char *name);

	/*
		Methods related to the retrieval of documents from the document repository.
	*/
	long get_longest_document_length(void) { return document_longest_raw_length; }
	char *get_document(char *destination, unsigned long *destination_length, long long id); // id is the document number
	long long get_document(char **destination, unsigned long **destination_length, long long from) { return get_documents(destination, destination_length, from, from + 1); }
	long long get_documents(char **destination, unsigned long **destination_length, long long from, long long to); // from and to are relative to the results list.
};

#endif  /* SEARCH_ENGINE_H_ */
