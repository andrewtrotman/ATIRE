/*
	RANKING_FUNCTION_LMDS.H
	----------------------
*/
#ifndef RANKING_FUNCTION_LMDS_H_
#define RANKING_FUNCTION_LMDS_H_

#include "ranking_function.h"

/*
	u=2500 is in the Petri et al (ADCS 2013) paper, I have no idea where they got it from.
*/
#define ANT_RANKING_FUNCTION_LMDS_DEFAULT_U 2500.0

/*
	class ANT_RANKING_FUNCTION_LMDS
	------------------------------
*/
class ANT_ranking_function_lmds : public ANT_ranking_function
{
private:
	double u;

public:
	ANT_ranking_function_lmds(ANT_search_engine *engine, long quantize, long long quantization_bits, double u = ANT_RANKING_FUNCTION_LMDS_DEFAULT_U) : ANT_ranking_function(engine, quantize, quantization_bits) { this->u = u; }
	ANT_ranking_function_lmds(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double u = ANT_RANKING_FUNCTION_LMDS_DEFAULT_U) : ANT_ranking_function(documents, document_lengths, quantization_bits) { this->u = u; }
	virtual ~ANT_ranking_function_lmds() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency);
	virtual double score_one_document(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency, double terms_in_query);
} ;

#endif
