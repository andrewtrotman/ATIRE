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
	ANT_ranking_function_readability(ANT_search_engine *engine, double mix = 0, double cutoff = 0, double k1 = 0.9, double b = 0.4);
	virtual ~ANT_ranking_function_readability() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency);
} ;


#endif  /* RANKING_FUNCTION_READABILITY_H_ */
