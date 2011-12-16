
/*
	RANKING_FUNCTION_PREGEN.H
	-----------------------------
*/
#ifndef RANKING_FUNCTION_PREGEN_H_
#define RANKING_FUNCTION_PREGEN_H_

#include "ranking_function.h"
#include "pregen.h"

/*
	class ANT_RANKING_FUNCTION_DOCID
	-------------------------------------
*/
class ANT_ranking_function_pregen : public ANT_ranking_function
{
private:
	ANT_pregen *pregen;
	long ascending;

public:
	ANT_ranking_function_pregen(ANT_search_engine *engine, ANT_pregen *pregen, long ascending) : ANT_ranking_function(engine), pregen(pregen), ascending(ascending) {};
	ANT_ranking_function_pregen(long long documents, ANT_compressable_integer *document_lengths, long ascending) : ANT_ranking_function(documents, document_lengths), pregen(NULL), ascending(ascending) {}
	virtual ~ANT_ranking_function_pregen() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;


#endif /* RANKING_FUNCTION_DOCID_H_ */
