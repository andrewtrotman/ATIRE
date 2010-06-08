/*
	MEMORY_INDEXER_NODE.H
	---------------------
*/
#ifndef MEMORY_INDEXER_NODE_H_
#define MEMORY_INDEXER_NODE_H_

#include "memory.h"
#include "string_pair.h"

/*
	class ANT_MEMORY_INDEXER_NODE
	-----------------------------
*/
class ANT_memory_indexer_node
{
public:
	ANT_string_pair string;
	long long term_frequency;

public:
	void *operator new(size_t count, ANT_memory *memory) { return memory->malloc(count); }
	ANT_memory_indexer_node() { term_frequency = 0; }
} ;

#endif /* MEMORY_INDEXER_NODE_H_ */
