/*
	RANKING_FUNCTION_LMJM.H
	-----------------------
*/
#ifndef RANKING_FUNCTION_LMJM_H_
#define RANKING_FUNCTION_LMJM_H_

#include "ranking_function.h"
#define ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA 0.9

/*
	class ANT_RANKING_FUNCTION_LMJM
	-------------------------------
*/
class ANT_ranking_function_lmjm : public ANT_ranking_function
{
private:
	double lambda;

public:
	ANT_ranking_function_lmjm(ANT_search_engine *engine, double lambda = ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA) : ANT_ranking_function(engine) { this->lambda = lambda; }
	ANT_ranking_function_lmjm(long long documents, ANT_compressable_integer *document_lengths, double lambda = ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA) : ANT_ranking_function(documents, document_lengths) { this->lambda = lambda; }
	virtual ~ANT_ranking_function_lmjm() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;


#endif  /* RANKING_FUNCTION_LMJM_H_ */
