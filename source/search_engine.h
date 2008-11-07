/*
	SEARCH_ENGINE.H
	---------------
*/
#ifndef __SEARCH_ENGINE_H__
#define __SEARCH_ENGINE_H__

#include "search_engine_btree_leaf.h"

class ANT_memory;
class ANT_file;
class ANT_search_engine_btree_node;

class ANT_search_engine
{
private:
	ANT_search_engine_btree_leaf term_details;
	ANT_memory *memory;
	ANT_file *index;
	ANT_search_engine_btree_node *btree_root;
	long btree_nodes;
	unsigned char *btree_leaf_buffer;

private:
	long long get_long_long(unsigned char *from) { return *((long long *)from); }
	long get_long(unsigned char *from) { return *((long *)from); }

public:
	ANT_search_engine(ANT_memory *memory);
	~ANT_search_engine();

	long long get_btree_leaf_position(char *term, long long *length, long *exact_match);
	ANT_search_engine_btree_leaf *get_postings_details(char *term);
};

#endif __SEARCH_ENGINE_H__
