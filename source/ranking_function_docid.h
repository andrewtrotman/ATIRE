
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
	ANT_ranking_function_docid(ANT_search_engine *engine, long quantize, long long quantization_bits, int ascending = 1) : ANT_ranking_function(engine, quantize, quantization_bits), ascending_order(ascending) {}
	ANT_ranking_function_docid(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, int ascending = 1) : ANT_ranking_function(documents, document_lengths, quantization_bits), ascending_order(ascending) {}
	virtual ~ANT_ranking_function_docid() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency);
} ;


#endif /* RANKING_FUNCTION_DOCID_H_ */
