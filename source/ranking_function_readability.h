/*
	RANKING_FUNCTION_READABILITY.H
	------------------------------
*/
#ifndef RANKING_FUNCTION_READABILITY_H_
#define RANKING_FUNCTION_READABILITY_H_

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
	double mix;
	double cutoff;
	long *document_readability;

public:
	ANT_ranking_function_readability(ANT_search_engine_readability *engine, double mix = 0, double cutoff = 0, double k1 = 0.9, double b = 0.4);
	virtual ~ANT_ranking_function_readability() {}

	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;


#endif  /* RANKING_FUNCTION_READABILITY_H_ */
