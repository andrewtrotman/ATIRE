/*
	RANKING_FUNCTION_IMPACT.H
	-------------------------
*/

#ifndef __RANKING_FUNCTION_IMPACT_H__
#define __RANKING_FUNCTION_IMPACT_H__

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_IMPACT
	---------------------------------
*/
class ANT_ranking_function_impact : public ANT_ranking_function
{
public:
	ANT_ranking_function_impact(ANT_search_engine *engine) : ANT_ranking_function(engine) {}; 
	virtual ~ANT_ranking_function_impact() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
} ;


#endif __RANKING_FUNCTION_IMPACT_H__
