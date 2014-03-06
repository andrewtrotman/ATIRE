/*
	RANKING_FUNCTION_LMJM.H
	-----------------------
*/
#ifndef RANKING_FUNCTION_LMJM_H_
#define RANKING_FUNCTION_LMJM_H_

#include "ranking_function.h"
#define ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA 0.5

/*
	class ANT_RANKING_FUNCTION_LMJM
	-------------------------------
*/
class ANT_ranking_function_lmjm : public ANT_ranking_function
{
private:
	double lambda;

public:
	ANT_ranking_function_lmjm(ANT_search_engine *engine, long quantize, long long quantization_bits, double lambda = ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA) : ANT_ranking_function(engine, quantize, quantization_bits) { this->lambda = lambda; }
	ANT_ranking_function_lmjm(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double lambda = ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA) : ANT_ranking_function(documents, document_lengths, quantization_bits) { this->lambda = lambda; }
	virtual ~ANT_ranking_function_lmjm() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency);
} ;


#endif  /* RANKING_FUNCTION_LMJM_H_ */
