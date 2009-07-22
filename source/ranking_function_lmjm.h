/*
	RANKING_FUNCTION_LMJM.H
	-----------------------
*/
#ifndef __RANKING_FUNCTION_LMJM_H__
#define __RANKING_FUNCTION_LMJM_H__

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_LMJM
	-------------------------------
*/
class ANT_ranking_function_lmjm : public ANT_ranking_function
{
private:
	double lambda;

public:
	ANT_ranking_function_lmjm(ANT_search_engine *engine, double lambda = 0.9) : ANT_ranking_function(engine) { this->lambda = lambda; }
	virtual ~ANT_ranking_function_lmjm() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
} ;


#endif __RANKING_FUNCTION_LMJM_H__
