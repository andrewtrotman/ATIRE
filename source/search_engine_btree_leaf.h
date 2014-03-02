/*
	SEARCH_ENGINE_BTREE_LEAF.H
	--------------------------
*/
#ifndef SEARCH_ENGINE_BTREE_LEAF_H_
#define SEARCH_ENGINE_BTREE_LEAF_H_

/*
	class ANT_SEARCH_ENGINE_BTREE_LEAF
	----------------------------------
*/
class ANT_search_engine_btree_leaf
{
public:
	long long local_collection_frequency;		// number of times the term occurs (in this index)
	long long global_collection_frequency;		// number of times the term occurs (in the distributed index)
	long long local_document_frequency;			// number of documents (in this index) that contain the term
	long long global_document_frequency;		// number of documents (in the distributed index) that contain the term
	long long postings_position_on_disk;		// where on the disk is the postings list stored?
	long long impacted_length;					// length (in integers) of the postings list (including impact scores)
	long long postings_length;					// length (in bytes) of the postings list (on disk)
#ifdef TERM_LOCAL_MAX_IMPACT
	unsigned char local_max_impact;				// the max impact score of this postings list
#endif
} ;

#endif  /* SEARCH_ENGINE_BTREE_LEAF_H_ */

