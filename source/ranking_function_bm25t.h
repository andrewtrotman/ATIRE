/*
	RANKING_FUNCTION_BM25T.H
	------------------------
*/
#ifndef RANKING_FUNCTION_BM25T_H_
#define RANKING_FUNCTION_BM25T_H_

#include "ranking_function.h"
#include "ranking_function_quantum_parameters.h"

/*
	Trained on the TREC WSJ collection topics 51-100
*/
#define ANT_RANKING_FUNCTION_BM25T_DEFAULT_B 0.2

/*
	class ANT_RANKING_FUNCTION_BM25T
	--------------------------------
*/
class ANT_ranking_function_BM25T : public ANT_ranking_function
{
private:
	double b;

private:
	double compute_k1(ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, double prescalar);

public:
	ANT_ranking_function_BM25T(ANT_search_engine *engine, long quantize, long long quantization_bits, double b = ANT_RANKING_FUNCTION_BM25T_DEFAULT_B);
	ANT_ranking_function_BM25T(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double b = ANT_RANKING_FUNCTION_BM25T_DEFAULT_B);
	virtual ~ANT_ranking_function_BM25T() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency);
} ;

#endif
