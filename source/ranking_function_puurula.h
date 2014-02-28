/*
	RANKING_FUNCTION_PUURULA.H
	--------------------------
*/
#ifndef RANKING_FUNCTION_PUURULA_H_
#define RANKING_FUNCTION_PUURULA_H_

#include "ranking_function.h"
#include "ranking_function_quantum_parameters.h"

/*
	No idea how Puurula trained these, but I think on WSJ
*/
#define ANT_RANKING_FUNCTION_PUURULA_U 1000.0
#define ANT_RANKING_FUNCTION_PUURULA_G 0.5

/*
	These were his original suggested parameters
*/
//#define ANT_RANKING_FUNCTION_PUURULA_U 0.001705343
//#define ANT_RANKING_FUNCTION_PUURULA_G 0.912423962

/*
	class ANT_RANKING_FUNCTION_PUURULA
	----------------------------------
*/
class ANT_ranking_function_puurula : public ANT_ranking_function
{
private:
	double g, u;
	double *discounted_document_lengths;

public:
	ANT_ranking_function_puurula(ANT_search_engine *engine, long quantize, long long quantization_bits, double u = ANT_RANKING_FUNCTION_PUURULA_U, double g =  ANT_RANKING_FUNCTION_PUURULA_G);
	ANT_ranking_function_puurula(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double u = ANT_RANKING_FUNCTION_PUURULA_U, double g = ANT_RANKING_FUNCTION_PUURULA_G) : ANT_ranking_function(documents, document_lengths, quantization_bits) { this->u = u; this->g = g; }
	virtual ~ANT_ranking_function_puurula() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency);
	virtual double score_one_document(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency, double terms_in_query);
} ;

#endif /* RANKING_FUNCTION_PUURULA_H_ */

