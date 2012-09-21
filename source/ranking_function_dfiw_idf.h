/*
	RANKING_FUNCTION_DFIW_IDF.H
	---------------------------
*/
#ifndef RANKING_FUNCTION_DFIW_IDF_H_
#define RANKING_FUNCTION_DFIW_IDF_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_DFIW_IDF
	-----------------------------------
*/
class ANT_ranking_function_DFIW_IDF : public ANT_ranking_function
{
public:
	ANT_ranking_function_DFIW_IDF(ANT_search_engine *engine) : ANT_ranking_function(engine) {puts("DFI_IDF");}
	ANT_ranking_function_DFIW_IDF(long long documents, ANT_compressable_integer *document_lengths) : ANT_ranking_function(documents, document_lengths) {}
	virtual ~ANT_ranking_function_DFIW_IDF() {}

	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;


#endif /* RANKING_FUNCTION_DFIW_IDF_H_ */
