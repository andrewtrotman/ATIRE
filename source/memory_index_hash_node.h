/*
	MEMORY_INDEX_HASH_NODE.H
	------------------------
*/

#ifndef __MEMORY_INDEX_HASH_NODE_H__
#define __MEMORY_INDEX_HASH_NODE_H__

#include "string_pair.h"

class ANT_memory;
class ANT_postings_piece;

class ANT_memory_index_hash_node
{
public:
	ANT_string_pair string;
	ANT_memory_index_hash_node *left, *right;
	ANT_postings_piece *docid_list_head, *docid_list_tail, *tf_list_head, *tf_list_tail;
	long long current_docno;
	ANT_memory *memory;

public:
	ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *string);
	~ANT_memory_index_hash_node();
	void *operator new(size_t count, ANT_memory *memory);
	void add_posting(long long docno);
} ;



#endif __MEMORY_INDEX_HASH_NODE_H__
