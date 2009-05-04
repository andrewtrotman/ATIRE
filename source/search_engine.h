/*
	SEARCH_ENGINE.H
	---------------
*/
#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include "search_engine_posting.h"
#include "compression_factory.h"

class ANT_memory;
class ANT_file;
class ANT_search_engine_btree_node;
class ANT_search_engine_btree_leaf;
class ANT_search_engine_accumulator;
class ANT_search_engine_stats;
class ANT_stemmer;

class ANT_search_engine
{
friend class ANT_btree_iterator;
friend class ANT_mean_average_precision;
private:
	ANT_search_engine_stats *stats;
	ANT_memory *memory;
	ANT_file *index;
	ANT_search_engine_btree_node *btree_root;
	long btree_nodes;
	long long documents;
	long *document_lengths;
	double mean_document_length;
	unsigned char *btree_leaf_buffer, *postings_buffer;
	ANT_search_engine_accumulator *accumulator;
	ANT_search_engine_accumulator **accumulator_pointers;
	ANT_search_engine_posting posting;
	long long max_header_block_size;
	long string_length_of_longest_term;
	long long highest_df;
	long *stem_buffer;
	long long stem_buffer_length_in_bytes;

private:
	long long get_long_long(unsigned char *from) { return *((long long *)from); }
	long get_long(unsigned char *from) { return *((long *)from); }
	ANT_search_engine_btree_leaf *get_leaf(unsigned char *leaf, long term_in_leaf, ANT_search_engine_btree_leaf *term_details);

protected:
	virtual void relevance_rank(ANT_search_engine_btree_leaf *leaf, ANT_search_engine_posting *postings);

public:
	ANT_search_engine(ANT_memory *memory);
	virtual ~ANT_search_engine();

	void init_accumulators(void);
	long long get_btree_leaf_position(char *term, long long *length, long *exact_match, long *btree_root_node);
	ANT_search_engine_btree_leaf *get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details);
	unsigned char *get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination);

#ifdef ANT_COMPRESS_EXPERIMENT
	ANT_compression_factory factory;
#endif

	void decompress(unsigned char *start, unsigned char *end, long *into, ANT_search_engine_btree_leaf *leaf);
	void decompress_tf(unsigned char *start, unsigned char *end, long *into);
	void stem_to_postings(ANT_search_engine_btree_leaf *stemmed_term_details, ANT_search_engine_posting  *posting, long long collection_frequency, long *stem_buffer);
	void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term);
	void process_one_search_term(char *term);
	ANT_search_engine_accumulator *sort_results_list(long long accurrate_rank_point, long long *hits);
	char **generate_results_list(char **document_id_list, char **sorted_id_list, long long top_k);
	long long document_count(void) { return documents; }
	void stats_initialise(void);
	void stats_text_render(void);
};

#endif __SEARCH_ENGINE_H__
