
/*
	RANKING_FUNCTION_DOCID.H
	-----------------------------
*/
#ifndef RANKING_FUNCTION_DOCID_H_
#define RANKING_FUNCTION_DOCID_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_DOCID
	-------------------------------------
*/
class ANT_ranking_function_docid : public ANT_ranking_function
{
private:
	int ascending_order;

public:
	ANT_ranking_function_docid(ANT_search_engine *engine, int ascending = 1) : ANT_ranking_function(engine), ascending_order(ascending) {}
	ANT_ranking_function_docid(long long documents, ANT_compressable_integer *document_lengths, int ascending = 1) : ANT_ranking_function(documents, document_lengths), ascending_order(ascending) {}
	virtual ~ANT_ranking_function_docid() {}

	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;


#endif /* RANKING_FUNCTION_DOCID_H_ */
