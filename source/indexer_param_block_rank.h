/*
	INDEXER_PARAM_BLOCK_RANK.H
	--------------------------
*/
#ifndef INDEXER_PARAM_BLOCK_RANK_H_
#define INDEXER_PARAM_BLOCK_RANK_H_

#include "ranking_function_factory.h"

class ANT_ranking_function;

/*
	class ANT_INDEXER_PARAM_BLOCK_RANK
	----------------------------------
*/
class ANT_indexer_param_block_rank : public ANT_ranking_function_factory
{
public:
	enum { BM25 = 1, IMPACT = 2, READABLE = 4, LMD = 8, LMJM = 16, BOSE_EINSTEIN = 32, DIVERGENCE = 64, TERM_COUNT = 128, ALL_TERMS = 256, INNER_PRODUCT = 512, KBTFIDF = 1024, DLH13 = 2048, DOCID = 4096};

protected:
	static const long long index_functions =  BM25 | IMPACT | LMD | LMJM | BOSE_EINSTEIN | DIVERGENCE | TERM_COUNT | INNER_PRODUCT | KBTFIDF | DLH13 | DOCID;
	static const long long search_functions = BM25 | IMPACT | READABLE | LMD | LMJM | BOSE_EINSTEIN | DIVERGENCE | TERM_COUNT | ALL_TERMS | INNER_PRODUCT | KBTFIDF | DLH13 | DOCID;

public:
	long ranking_function;				// what ranking function should we use?

	double lmd_u;						// the u value for Language Modelling with Dirichlet smoothing
	double lmjm_l;						// the l (lamda) value for Language Modelling with Jelinek-Mercer smoothing
	double bm25_k1, bm25_b;				// the k1 and b value for BM25
	double kbtfidf_k, kbtfidf_b;		// the k and b paramters for KBTFIDF

	int ascending;						// ascending/descending switch for docid ranking

private:
	const char *isdefault(long long what) { return ranking_function == what ? "[default]" : ""; }

protected:
	ANT_indexer_param_block_rank();
	virtual ~ANT_indexer_param_block_rank() {}

	virtual void get_two_parameters(char *from, double *first, double *second);
	virtual void get_one_parameter(char *from, double *into);

	virtual void set_ranker(char *which);
	virtual void help(char *title, char switch_char, long long allowable);

	virtual ANT_ranking_function *get_indexing_ranker(long long documents, ANT_compressable_integer *lengths);
} ;

#endif /* INDEXER_PARAM_BLOCK_RANK_H_ */
