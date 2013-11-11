/*
	RANKING_FUNCTION_BM25.H
	-----------------------
*/
#ifndef __RANKING_FUNCTION_BM25_H__
#define __RANKING_FUNCTION_BM25_H__

#include "ranking_function.h"
#include "ranking_function_quantum_parameters.h"

/*
	Trained on the INEX Wikipedia Collection and the 2009 query set (k1=0.9, b=0.4)
*/
#define ANT_RANKING_FUNCTION_BM25_DEFAULT_K1  0.9
#define ANT_RANKING_FUNCTION_BM25_DEFAULT_B   0.4

/*
	class ANT_RANKING_FUNCTION_BM25
	-------------------------------
*/
class ANT_ranking_function_BM25 : public ANT_ranking_function
{
private:
	double k1, b;
	float *document_prior_probability;

private:
	void init(double k1, double b);

public:
	ANT_ranking_function_BM25(ANT_search_engine *engine, long quantize, long long quantization_bits, double k1 = ANT_RANKING_FUNCTION_BM25_DEFAULT_K1, double b = ANT_RANKING_FUNCTION_BM25_DEFAULT_B) : ANT_ranking_function(engine, quantize, quantization_bits) { init(k1, b); }
	ANT_ranking_function_BM25(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double k1 = ANT_RANKING_FUNCTION_BM25_DEFAULT_K1, double b = ANT_RANKING_FUNCTION_BM25_DEFAULT_B) : ANT_ranking_function(documents, document_lengths, quantization_bits) { init(k1, b); }
	virtual ~ANT_ranking_function_BM25();

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency);
} ;

#endif  /* __RANKING_FUNCTION_BM25_H__ */
