/*
	MEMORY_INDEX_ONE_NODE.H
	-----------------------
*/
#ifndef MEMORY_INDEX_ONE_NODE_H_
#define MEMORY_INDEX_ONE_NODE_H_

#include "memory_indexer.h"

class ANT_memory_index_hash_node;

/*
	class ANT_MEMORY_INDEX_ONE_NODE
	-------------------------------
	This class will initially be used for indexing documents in parallel, but it won't
	be long before it gets use for two other purposes.  For Relevance Feedback we need
	a way to store the term details.  For KL-divergence we need the same.  For query
	performance prediction we need a way to store document details too.  For this reason
	the term_frequency is a long rather than an unsigned char.  term_frequency is later
	capped at 8 bits.
*/
class ANT_memory_index_one_node : public ANT_memory_indexer_node
{
public:
	ANT_memory_index_one_node *left, *right;	// for hash table direct chaining
	long mode;											// the mode used in set_document_detail (only applicable when string[0]='~');
	ANT_memory_index_hash_node *final_node;	// the node from the ANT_memory_index structure into which this term is later added
	double kl_score;									// the KL divergence score for this term

public:
	ANT_memory_index_one_node() : ANT_memory_indexer_node(), left(NULL), right(NULL), mode(0), final_node(NULL), kl_score(0.0) {}

	/*
		This method is used for building the heap
	*/
	inline int operator() (ANT_memory_index_one_node *a, ANT_memory_index_one_node *b) { return a->kl_score < b->kl_score ? -1 : a->kl_score == b->kl_score ? 0 : 1; }
} ;

#endif /* MEMORY_INDEX_ONE_NODE_H_ */

