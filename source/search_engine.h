/*
	SEARCH_ENGINE.H
	---------------
*/
#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include "compression_factory.h"

class ANT_memory;
class ANT_file;
class ANT_search_engine_btree_node;
class ANT_search_engine_btree_leaf;
class ANT_search_engine_accumulator;
class ANT_search_engine_stats;
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

private:
	ANT_search_engine_stats *stats;
	ANT_search_engine_stats *stats_for_all_queries;
	ANT_memory *memory;
	ANT_file *index;
	ANT_search_engine_btree_node *btree_root;
	long btree_nodes;
	long long max_header_block_size;
	long string_length_of_longest_term;
	long long highest_df;

protected:
	ANT_compressable_integer *decompress_buffer;
	ANT_compression_factory factory;
	long long documents;
	unsigned char *btree_leaf_buffer, *postings_buffer;
	long *stem_buffer;
	long *document_lengths;
	double mean_document_length;
	ANT_search_engine_accumulator *accumulator;
	ANT_search_engine_accumulator **accumulator_pointers;
	long long stem_buffer_length_in_bytes;
	long long trim_postings_k;
	long long collection_length_in_terms;

private:
	long long get_long_long(unsigned char *from) { return *((int64_t *)from); }
	long get_long(unsigned char *from) { return *((int32_t *)from); }
	ANT_search_engine_btree_leaf *get_leaf(unsigned char *leaf, long term_in_leaf, ANT_search_engine_btree_leaf *term_details);

public:
	ANT_search_engine(ANT_memory *memory);
	virtual ~ANT_search_engine();

	void init_accumulators(void);
	long long get_btree_leaf_position(char *term, long long *length, long *exact_match, long *btree_root_node);
	ANT_search_engine_btree_leaf *get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details);
	unsigned char *get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination);

	virtual void process_one_search_term(char *term, ANT_ranking_function *ranking_function);
	virtual void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function);
	ANT_search_engine_accumulator *sort_results_list(long long accurrate_rank_point, long long *hits);
	char **generate_results_list(char **document_id_list, char **sorted_id_list, long long top_k);
	long long document_count(void) { return documents; }
	long *get_document_lengths(double *mean) { *mean = mean_document_length; return document_lengths; }
	ANT_compressable_integer *get_decompress_buffer(void) { return decompress_buffer; }
	ANT_search_engine_stats *get_stats(void) { return stats; }
	void stats_initialise(void);
	void stats_text_render(void);
	void stats_all_text_render(void);
	void stats_add(void);
	void set_trim_postings_k(long long what) { trim_postings_k = what; }
	long long get_collection_length(void) { return collection_length_in_terms; }

	ANT_compressable_integer *get_decompressed_postings(char *term, ANT_search_engine_btree_leaf *term_details);
};

#endif __SEARCH_ENGINE_H__
