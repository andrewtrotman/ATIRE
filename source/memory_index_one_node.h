/*
	MEMORY_INDEX_ONE_NODE.H
	-----------------------
*/
#ifndef MEMORY_INDEX_ONE_NODE_H_
#define MEMORY_INDEX_ONE_NODE_H_

#include "memory_indexer.h"

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
	ANT_memory_index_one_node *left, *right;

public:
	ANT_memory_index_one_node() : ANT_memory_indexer_node () { left = right = NULL; }
} ;

#endif /* MEMORY_INDEX_ONE_NODE_H_ */

