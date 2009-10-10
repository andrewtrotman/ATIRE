/*
	INDEXER_PARAM_BLOCK_RANK.H
	--------------------------
*/
#ifndef INDEXER_PARAM_BLOCK_RANK_H_
#define INDEXER_PARAM_BLOCK_RANK_H_

class ANT_indexer_param_block_rank
{
public:
	enum { BM25, IMPACT, READABLE, LMD, LMJM, BOSE_EINSTEIN, DIVERGENCE};

public:
	long ranking_function;				// what ranking function should we use?

	double lmd_u;						// the u value for Language Modelling with Dirichlet smoothing
	double lmjm_l;						// the l (lamda) value for Language Modelling with Jelinek-Mercer smoothing
	double bm25_k1, bm25_b;				// the k1 and b value for BM25

protected:
	ANT_indexer_param_block_rank();
	virtual ~ANT_indexer_param_block_rank() {}

	virtual void get_two_parameters(char *from, double *first, double *second);
	virtual void get_one_parameter(char *from, double *into);

	virtual void set_ranker(char *which);
	virtual void help(char *title, char switch_char);
} ;


#endif /* INDEXER_PARAM_BLOCK_RANK_H_ */
