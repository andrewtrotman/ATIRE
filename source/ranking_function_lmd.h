/*
	RANKING_FUNCTION_LMD.H
	----------------------
*/
#ifndef RANKING_FUNCTION_LMD_H_
#define RANKING_FUNCTION_LMD_H_

#include "ranking_function.h"

#define ANT_RANKING_FUNCTION_LMD_DEFAULT_U 500.0
/*
	class ANT_RANKING_FUNCTION_LMD
	------------------------------
*/
class ANT_ranking_function_lmd : public ANT_ranking_function
{
private:
	double u;

public:
	ANT_ranking_function_lmd(ANT_search_engine *engine, double u = ANT_RANKING_FUNCTION_LMD_DEFAULT_U) : ANT_ranking_function(engine) { this->u = u; }
	ANT_ranking_function_lmd(long long documents, ANT_compressable_integer *document_lengths, double u = ANT_RANKING_FUNCTION_LMD_DEFAULT_U) : ANT_ranking_function(documents, document_lengths) { this->u = u; }
	virtual ~ANT_ranking_function_lmd() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator_array *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;

#endif  /* RANKING_FUNCTION_LMD_H_ */

