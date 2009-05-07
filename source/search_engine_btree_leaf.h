/*
	SEARCH_ENGINE_BTREE_LEAF.H
	--------------------------
*/
#ifndef __SEARCH_ENGINE_BTREE_LEAF_H__
#define __SEARCH_ENGINE_BTREE_LEAF_H__

class ANT_search_engine_btree_leaf
{
public:
	long long collection_frequency;
	long document_frequency;
	long long postings_position_on_disk;
	long impacted_length;
	long postings_length;
} ;

#endif __SEARCH_ENGINE_BTREE_LEAF_H__
