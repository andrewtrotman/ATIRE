/*
	RANKING_FUNCTION_SIMILARITY.H
	-----------------------------
*/
#ifndef RANKING_FUNCTION_SIMILARITY_H_
#define RANKING_FUNCTION_SIMILARITY_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_SIMILARITY
	-------------------------------------
*/
class ANT_ranking_function_similarity : public ANT_ranking_function
{
private:
	double k1;
	double b;
	float *document_prior_probability;

public:
	ANT_ranking_function_similarity(ANT_search_engine *engine, double k1 = 0.9, double b = 0.4);
	virtual ~ANT_ranking_function_similarity();
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);

	virtual void relevance_rank_tf(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array, long long trim_point, double prescalar, double postscalar);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;

#endif  /* RANKING_FUNCTION_SIMILARITY_H_ */
