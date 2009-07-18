/*
	RANKING_FUNCTION_BM25.H
	-----------------------
*/
#ifndef __RANKING_FUNCTION_BM25_H__
#define __RANKING_FUNCTION_BM25_H__

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_BM25
	-------------------------------
*/
class ANT_ranking_function_BM25 : public ANT_ranking_function
{
private:
	double k1;
	double b;

public:
	ANT_ranking_function_BM25(ANT_search_engine *engine, double k1 = 0.9, double b = 0.4) : ANT_ranking_function(engine) { this->k1 = k1; this->b = b; }
	virtual ~ANT_ranking_function_BM25() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering);
	virtual void relevance_rank_tf(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, long *tf_array);
} ;

#endif __RANKING_FUNCTION_BM25_H__
