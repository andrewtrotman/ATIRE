/*
	MEMORY_INDEX.H
	--------------
*/
#ifndef MEMORY_INDEX_H_
#define MEMORY_INDEX_H_

#include "memory_indexer.h"
#include "stats_memory_index.h"
#include "memory_index_hash_node.h"
#include "fundamental_types.h"
#include "compress_variable_byte.h"
#include "compression_factory.h"
#include "compression_text_factory.h"
#include "hash_table.h"
#include "impact_header.h"
#include "version.h"
#include "stop_word.h"

class ANT_memory_index_hash_node;
class ANT_memory;
class ANT_memory_index_one;
class ANT_memory_index_one_node;
class ANT_string_pair;
class ANT_file;
class ANT_ranking_function;
class ANT_ranking_function_factory;

/*
	class ANT_MEMORY_INDEX
	----------------------
*/
class ANT_memory_index : public ANT_memory_indexer
{
friend class ANT_memory_index_one;
friend class ANT_search_engine_memory_index;

public:
	static const long HASH_TABLE_SIZE = 0x1000000;

public:
	enum { STAT_MEMORY = 1, STAT_TIME = 2, STAT_COMPRESSION = 4, STAT_SUMMARY = 8 };
	enum { NONE = 0, PRUNE_CF_SINGLETONS = 1, PRUNE_DF_SINGLETONS = 2, PRUNE_TAGS = 4, PRUNE_NUMBERS = 8, PRUNE_DF_FREQUENTS = 16, PRUNE_NCBI_STOPLIST = 32 };

private:
	long hashed_squiggle_length;
	ANT_stop_word stopwords;
public:
	ANT_memory_index_hash_node *hash_table[HASH_TABLE_SIZE];
private:
	ANT_memory *dictionary_memory, *postings_memory, *serialisation_memory, *titles_memory;
	unsigned char *serialised_docids;
	unsigned short *serialised_tfs;
	long long serialised_docids_size, serialised_tfs_size;
	long long largest_docno;
	ANT_stats_memory_index *stats;

	ANT_compress_variable_byte variable_byte;
	ANT_compression_factory *factory;
	ANT_compressable_integer *decompressed_postings_list, *impacted_postings;
	unsigned char *compressed_postings_list;
	long long compressed_postings_list_length;

	long long static_prune_point;					// this is the maximum number of postings allowed in an impact (or tf) ordered postings list
	long stop_word_removal_mode;					// remove cf-singletons or df-singletons (etc.)
	long stop_word_df_frequencies;				// remove words based on document frequencies
	double stop_word_max_proportion;				// remove words that have a dl/N >= this value

	ANT_file *index_file;

	/*
		These are used for quantization.  We need to store the max and min rsv values
		so that we can scale them.  We also need the document lengths array and some
		form of quantizer
	*/
	ANT_compressable_integer *document_lengths;
	ANT_ranking_function *quantizer;
	long long quantization_bits;
	long index_quantization;
	double maximum_collection_rsv, minimum_collection_rsv;

	/*
		Documents that are added to the repository are compressed using the compression
		factory first.  A list of offsets is stored in the index under the name ~documentoffsets
		the length of the longest document (once decompressed) is stored as ~documentlongest
	*/
	long long documents_in_repository;
	long compressed_longest_raw_document_size;			// length of the longest document before compression
	static ANT_string_pair squiggle_document_offsets;
	static ANT_string_pair squiggle_document_longest;

	/*
		Document filenames are stored in a big buffer after the documents but before the index
	*/
	char *document_filenames;
	static const long long document_filenames_chunk_size = (1024 * 1024);
	long long document_filenames_used;

#ifdef IMPACT_HEADER
	ANT_impact_header impact_header;
	long long compressed_impact_header_size;
	unsigned char *compressed_impact_header_buffer;
	long long impact_value_size;
	long long doc_count_size;
	long long impact_offset_size;
#endif
	/*
		These are needed for impact_order() and impact_order_with_header()
	*/
	ANT_compressable_integer bucket_size[1 << 16], bucket_prev_docid[1 << 16], *pointer[1 << 16];

private:
	static long hash(ANT_string_pair *string) { return ANT_hash_24(string); }
	ANT_memory_index_hash_node *find_node(ANT_memory_index_hash_node *root, ANT_string_pair *string);
	ANT_memory_index_hash_node *find_add_node(ANT_memory_index_hash_node *root, ANT_string_pair *string);
	long serialise_all_nodes(ANT_file *file, ANT_memory_index_hash_node *root);
	ANT_memory_index_hash_node *new_memory_index_hash_node(ANT_string_pair *string);
public:
	long generate_term_list(ANT_memory_index_hash_node *root, ANT_memory_index_hash_node **into, long where, int32_t *length_of_longest_term, int64_t *highest_df);
	ANT_memory_index_hash_node **find_end_of_node(ANT_memory_index_hash_node **start);
	ANT_memory_index_hash_node **write_node(ANT_file *file, ANT_memory_index_hash_node **start);
private:
	long long node_to_postings(ANT_memory_index_hash_node *root);
#ifdef IMPACT_HEADER
	long long impact_order_with_header(ANT_compressable_integer *destination, ANT_compressable_integer *docid, unsigned short *term_frequency, long long document_frequency, unsigned char *max_local);
#endif
	long long impact_order(ANT_compressable_integer *destination, ANT_compressable_integer *docid, unsigned short *term_frequency, long long document_frequency, unsigned char *max_local);
	double rsv_all_nodes(double *minimum, ANT_memory_index_hash_node *root);
	long long get_serialised_postings(ANT_memory_index_hash_node *root, long long *doc_size, long long *tf_size);
	void open_index_file(char *filename);
	void close_index_file(void);
	void add_to_filename_repository(char *filename);
	void serialise_filenames(char *source,  long depth = 0);

	void add_indexed_document_node(ANT_memory_index_one_node *node, long long docno);

	long should_prune(ANT_memory_index_hash_node *term);

	void text_render(ANT_memory_index_hash_node *root, unsigned char *serialised_docids, long doc_size, unsigned short *serialised_tfs, long tf_size);
	void text_render(ANT_compressable_integer *impact_ordering, size_t document_frequency);
	void text_render(ANT_compressable_integer *docid, unsigned char *term_frequency, long long document_frequency);

public:
	ANT_memory_index(char *filename);
	virtual ~ANT_memory_index();

	void set_variable(char *measure_name, long long score);
	void set_variable(ANT_string_pair *measure_name, long long score);

	void text_render(long what);
	void set_compression_scheme(unsigned long scheme) { factory->set_scheme(scheme); }
	void set_compression_validation(unsigned long validate) { factory->set_validation(validate); }

	void add_to_document_repository(char *filename, char *compressed_document, long compressed_length, long length);
	long serialise(ANT_ranking_function_factory *factory);

	void add_indexed_document(ANT_memory_index_one *index, long long docno);

	virtual ANT_memory_index_hash_node *add_term(ANT_string_pair *string, long long docno, long term_frequency = 1);
	virtual long long get_memory_usage(void) { return dictionary_memory->bytes_used() + postings_memory->bytes_used(); }
	virtual void set_document_length(long long docno, long long length) { set_document_detail(&squiggle_length, length); largest_docno = docno; }
	virtual void set_document_detail(ANT_string_pair *measure_name, long long length, long mode = MODE_ABSOLUTE);
	virtual void set_static_pruning(long long k) { static_prune_point = k; }
	virtual void set_term_culling(long mode, double max_df, long df) {stop_word_removal_mode = mode; stop_word_max_proportion = max_df; stop_word_df_frequencies = df; }
} ;

/*
	ANT_MEMORY_INDEX::NEW_MEMORY_INDEX_HASH_NODE()
	----------------------------------------------
*/
inline ANT_memory_index_hash_node *ANT_memory_index::new_memory_index_hash_node(ANT_string_pair *string)
{
stats->unique_terms++;

return new (dictionary_memory) ANT_memory_index_hash_node(dictionary_memory, postings_memory, string, stats);
}

#endif  /* MEMORY_INDEX_H_ */
