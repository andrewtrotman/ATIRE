/*
	NEXI_TERM_ANT.H
	---------------
*/
#ifndef NEXI_TERM_ANT_H_
#define NEXI_TERM_ANT_H_

#include "NEXI_term.h"
#include "search_engine_btree_leaf.h"

/*
	class ANT_NEXI_TERM_ANT
	-----------------------
*/
class ANT_NEXI_term_ant : public ANT_NEXI_term
{
public:
	ANT_search_engine_btree_leaf term_details; 	// position of the postings on the disk
	 
public:
	static int cmp_collection_frequency(const void *a, const void *b);
	static int cmp_local_max_impact(const void *a, const void *b);
	static int cmp_term(const void *a, const void *b);
} ;

#endif /* NEXI_TERM_ANT_H_ */

