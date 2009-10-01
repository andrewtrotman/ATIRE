/*
	RANKING_FUNCTION_BOSE_EINSTEIN.H
	--------------------------------
*/
#ifndef RANKING_FUNCTION_BOSE_EINSTEIN_H_
#define RANKING_FUNCTION_BOSE_EINSTEIN_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_BOSE_EINSTEIN
	----------------------------------------
*/
class ANT_ranking_function_bose_einstein : public ANT_ranking_function
{
public:
	ANT_ranking_function_bose_einstein(ANT_search_engine *engine) : ANT_ranking_function(engine) { }
	virtual ~ANT_ranking_function_bose_einstein() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
} ;

#endif  /* RANKING_FUNCTION_BOSE_EINSTEIN_H_ */

