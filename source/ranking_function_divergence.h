/*
	RANKING_FUNCTION_DIVERGENCE.H
	-----------------------------
*/
#ifndef RANKING_FUNCTION_DIVERGENCE_H_
#define RANKING_FUNCTION_DIVERGENCE_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_DIVERGENCE
	-------------------------------------
*/
class ANT_ranking_function_divergence : public ANT_ranking_function
{
public:
	ANT_ranking_function_divergence(ANT_search_engine *engine) : ANT_ranking_function(engine) {}
	ANT_ranking_function_divergence(long long documents, ANT_compressable_integer *document_lengths) : ANT_ranking_function(documents, document_lengths) {}
	virtual ~ANT_ranking_function_divergence() {}

	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;

#endif  /* RANKING_FUNCTION_DIVERGENCE_H_ */
