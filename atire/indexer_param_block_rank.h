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
	enum { BM25 = 0x0001, IMPACT = 0x0002, READABLE = 0x0004, LMD = 0x0008, LMJM = 0x0010, BOSE_EINSTEIN = 0x0020, DIVERGENCE = 0x0040, TERM_COUNT = 0x0080, ALL_TERMS = 0x0100, INNER_PRODUCT = 0x0200, KBTFIDF = 0x0400, DLH13 = 0x0800, DOCID = 0x1000, PREGEN = 0x2000, DPH = 0x4000, DFREE = 0x8000, DFI = 0x10000, DFIW = 0x20000, DFI_IDF = 0x40000, DFIW_IDF = 0x80000};	// we get 64 bits here

protected:
	static const long long index_functions =  BM25 | IMPACT | LMD | LMJM | BOSE_EINSTEIN | DIVERGENCE | TERM_COUNT | INNER_PRODUCT | KBTFIDF | DLH13 | DPH | DFREE | DOCID | DPH | DFREE | DFI;
	static const long long search_functions = BM25 | IMPACT | READABLE | LMD | LMJM | BOSE_EINSTEIN | DIVERGENCE | TERM_COUNT | ALL_TERMS | INNER_PRODUCT | KBTFIDF | DLH13 | DOCID | PREGEN | DPH | DFREE | DFI;

public:
	long long ranking_function;			// what ranking function should we use?

	double lmd_u;						// the u value for Language Modelling with Dirichlet smoothing
	double lmjm_l;						// the l (lamda) value for Language Modelling with Jelinek-Mercer smoothing
	double bm25_k1, bm25_b;				// the k1 and b value for BM25
	double kbtfidf_k, kbtfidf_b;		// the k and b paramters for KBTFIDF

	int ascending;						// ascending/descending switch for docid ranking
	char *field_name;					// field to rank on for pregens
private:
	const char *isdefault(long long what) { return ranking_function == what ? "[default]" : ""; }

protected:
	virtual int get_two_parameters(char *from, double *first, double *second);
	virtual int get_one_parameter(char *from, double *into);

	virtual void help(char *title, char switch_char, long long allowable);

	virtual ANT_ranking_function *get_indexing_ranker(long long documents, ANT_compressable_integer *lengths);
public:
	ANT_indexer_param_block_rank();
	virtual ~ANT_indexer_param_block_rank();

	virtual int set_ranker(char *which);
} ;

#endif /* INDEXER_PARAM_BLOCK_RANK_H_ */
