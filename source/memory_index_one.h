/*
	MEMORY_INDEX_ONE.H
	------------------
*/
#ifndef MEMORY_INDEX_ONE_H_
#define MEMORY_INDEX_ONE_H_

#include "memory_indexer.h"
#include "heap.h"	

class ANT_memory;
class ANT_memory_index_one_node;
class ANT_string_pair;
class ANT_memory_index;
class ANT_search_engine;
class ANT_search_engine_btree_leaf;
class ANT_term_divergence;

/*
	class ANT_MEMORY_INDEX_ONE
	--------------------------
*/
class ANT_memory_index_one : public ANT_memory_indexer
{
friend class ANT_memory_index;

private:
	/*
		This is a careful ballence, it should be both large enough to avoid collisions of the terms in a
		single document, but small enough that itterating over the whole hash table will be fast, Otherwise
		the sequential cost post indexing will be too large.
	*/
	static const long HASH_TABLE_SIZE = 0x100;

	ANT_memory_index_one_node *hash_table[HASH_TABLE_SIZE];
	ANT_memory *memory;
	ANT_memory_index *final_index;
	long hashed_squiggle_length;
	long long document_length;
	long nodes_used;					// the number of unique terms in the index

	/*
		This stuff is used in the KL-divergence code to decrease the chance of stack blow-out
	*/
	ANT_search_engine_btree_leaf *term_details;
	char *token_as_string;

	/*
		This stuff is to do with computing good terms from documents
	*/
	ANT_memory_index_one_node **top_terms;
	ANT_heap<ANT_memory_index_one_node *, ANT_memory_index_one_node> *heap;
	long heap_terms;		// number of terms in the heap
	long heap_size;			// maximum size of the heap

private:
	ANT_memory_index_one_node *new_hash_node(ANT_string_pair *string);
	ANT_memory_index_one_node *find_node(ANT_memory_index_one_node *root, ANT_string_pair *string);
	ANT_memory_index_one_node *find_add_node(ANT_memory_index_one_node *root, ANT_string_pair *string);
	long hash(ANT_string_pair *string);
	ANT_memory_index_one_node *add(ANT_string_pair *string, long long docno, long extra_term_frequency);

	double kl_node(ANT_term_divergence *divergence, ANT_memory_index_one_node *node, ANT_search_engine *document_collection);
	double kl_node(ANT_term_divergence *divergence, ANT_memory_index_one_node *node, ANT_memory_index_one *document_collection);
	void top_terms_from_tree(ANT_memory_index_one_node *node);

	void add_term_to_table(ANT_memory_indexer_node **table, ANT_memory_index_one_node *node, long *term_id);

	void tree_get_frequencies(ANT_memory_index_one_node *node, short *frequency, long long tf_cap);

public:
	ANT_memory_index_one(ANT_memory *memory, ANT_memory_index *index = NULL);
	virtual ~ANT_memory_index_one();

	void rewind(void);

	virtual ANT_memory_indexer_node *add_term(ANT_string_pair *string, long long docno = 1, long extra_term_frequency = 1);
	virtual void set_document_length(long long docno, long long length) { set_document_detail(&squiggle_length, document_length = length); } 
//	virtual void set_puurula_length(double length) { set_document_detail(&squiggle_puurula_length, (long long)(length * 100)); /* accurate to 2 decimal places*/ }
	virtual long long get_memory_usage(void) { return memory->bytes_used(); }
 	virtual void set_document_detail(ANT_string_pair *measure_name, long long length, long mode = MODE_ABSOLUTE);
	virtual short *get_frequencies(short *frequency, long long tf_cap);

	ANT_memory_indexer_node **get_term_list(void);

	double kl_divergence(ANT_term_divergence *divergence, ANT_search_engine *collection);
	double kl_divergence(ANT_term_divergence *divergence, ANT_memory_index_one *collection);
	ANT_memory_index_one_node **top_n_terms(long terms_wanted, long *terms_found);
	long long get_document_length() { return document_length; }
	ANT_memory_index_one_node *get_term_node(ANT_string_pair *term);
} ;

#endif /* MEMORY_INDEX_ONE_H_ */

