/*
	RANKING_FUNCTION_BM25PLUS.H
	---------------------------
*/
#ifndef RANKING_FUNCTION_BM25PLUS_H_
#define RANKING_FUNCTION_BM25PLUS_H_

#include "ranking_function.h"
#include "ranking_function_quantum_parameters.h"

/*
	Trained on the INEX Wikipedia Collection and the 2009 query set (k1=0.9, b=0.4), and 1 is recommended in the paper
*/
#define ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_K1    0.9
#define ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_B     0.4
#define ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_DELTA 1

/*
	class ANT_RANKING_FUNCTION_BM25PLUS
	--------------------------------
*/
class ANT_ranking_function_BM25PLUS : public ANT_ranking_function
{
private:
	double k1, b, delta;

public:
	ANT_ranking_function_BM25PLUS(ANT_search_engine *engine, long quantize, long long quantization_bits, double k1 = ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_K1, double b = ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_B, double delta = ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_DELTA);
	ANT_ranking_function_BM25PLUS(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double k1 = ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_K1, double b = ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_B, double delta = ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_DELTA);
	virtual ~ANT_ranking_function_BM25PLUS() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency);
} ;

#endif
