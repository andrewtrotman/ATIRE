/*
	SEARCH_ENGINE_BTREE_LEAF.H
	--------------------------
*/
#ifndef SEARCH_ENGINE_BTREE_LEAF_H_
#define SEARCH_ENGINE_BTREE_LEAF_H_

class ANT_search_engine_btree_leaf
{
public:
	long long collection_frequency;
	long document_frequency;
	long long postings_position_on_disk;
	long impacted_length;
	long postings_length;
} ;

#endif  /* SEARCH_ENGINE_BTREE_LEAF_H_ */
