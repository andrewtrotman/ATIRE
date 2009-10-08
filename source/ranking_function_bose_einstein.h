/*
	RANKING_FUNCTION_BOSE_EINSTEIN.H
	--------------------------------
*/
#ifndef RANKING_FUNCTION_BOSE_EINSTEIN_H_
#define RANKING_FUNCTION_BOSE_EINSTEIN_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_BOSE_EINSTEIN
	----------------------------------------
*/
class ANT_ranking_function_bose_einstein : public ANT_ranking_function
{
public:
	ANT_ranking_function_bose_einstein(ANT_search_engine *engine) : ANT_ranking_function(engine) {}

#ifdef QUANTIZED_ORDERING
	ANT_ranking_function_bose_einstein(long long documents, ANT_compressable_integer *document_lengths) : ANT_ranking_function(documents, document_lengths) {}
#endif
	virtual ~ANT_ranking_function_bose_einstein() {}

	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);


#ifdef QUANTIZED_ORDERING
	virtual void get_max_min(double *maximum, double *minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned char *term_frequencies);
	virtual void quantize(double maximum, double minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned char *term_frequencies);
#endif
} ;

#endif  /* RANKING_FUNCTION_BOSE_EINSTEIN_H_ */

