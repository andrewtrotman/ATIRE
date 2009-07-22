/*
	RANKING_FUNCTION_LMD.H
	----------------------
*/
#ifndef __RANKING_FUNCTION_LMD_H__
#define __RANKING_FUNCTION_LMD_H__

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_LMD
	------------------------------
*/
class ANT_ranking_function_lmd : public ANT_ranking_function
{
private:
	double u;

public:
	ANT_ranking_function_lmd(ANT_search_engine *engine, double u = 500.0) : ANT_ranking_function(engine) { this->u = u; }
	virtual ~ANT_ranking_function_lmd() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
} ;

#endif __RANKING_FUNCTION_LMD_H__

