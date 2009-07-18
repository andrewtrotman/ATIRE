/*
	RANKING_FUNCTION_READABILITY.H
	------------------------------
*/
#ifndef __RANKING_FUNCTION_READABILITY_H__
#define __RANKING_FUNCTION_READABILITY_H__

#include "ranking_function.h"

class ANT_search_engine_readability;

/*
	class ANT_RANKING_FUNCTION_READABILITY
	--------------------------------------
*/
class ANT_ranking_function_readability : public ANT_ranking_function
{
private:
	double k1;
	double b;
	long *document_readability;

public:
	ANT_ranking_function_readability(ANT_search_engine_readability *engine, double k1 = 0.9, double b = 0.4);
	virtual ~ANT_ranking_function_readability() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering);
} ;


#endif __RANKING_FUNCTION_READABILITY_H__
