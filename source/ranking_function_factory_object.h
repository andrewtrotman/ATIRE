/*
	RANKING_FUNCTION_FACTORY_OBJECT.H
	---------------------------------
*/
#ifndef RANKING_FUNCTION_FACTORY_OBJECT_H_
#define RANKING_FUNCTION_FACTORY_OBJECT_H_

/*
	class ANT_RANKING_FUNCTION_FACTORY_OBJECT
	-----------------------------------------
*/
class ANT_ranking_function_factory_object
{
public:
	enum 
		{
		INDEXABLE = 1, 				// the ranking function can be used to generare a quantized index
		NONINDEXABLE = 2, 			// the ranking function cannot be used to generare a quantized index
		QUANTABLE = 4				// the ranking function can be run over an index quantized with a different ranking function (e.g. TERM_COUNT)
		};

	enum
		{
		NONE = 0x0000,
		BM25,
		BM25L,
		BM25PLUS,
		BM25T,
		BM25ADPT,
		TFLODOP,
		IMPACT,
		READABLE,
		LMD,
		LMJM,
		BOSE_EINSTEIN,
		DIVERGENCE,
		TERM_COUNT,
		ALL_TERMS,
		INNER_PRODUCT,
		KBTFIDF,
		DLH13,
		DOCID,
		PREGEN,
		DPH,
		DFREE,
		DFI,
		DFIW,
		DFI_IDF,
		DFIW_IDF,
		LMDS,
		PUURULA,
		PUURULA_IDF
		};

public:
	long id;						// BM25
	char *name;						// "BM25"
	char *parameters;				// "<k1>:<b>"
	long flags;						// can be used at indexing time (INDEXABLE) can run on a quanitized index (nope)
	long parameter_count;			// 2 parameters
	double p1;						// default k1
	double p2;						// default b
	double p3;						// default delta
	double feedback_p1;				// default k1 when relevance feedback is used
	double feedback_p2;				// default b when relevance feedback is used
	double feedback_p3;				// default delta when relevance feedback is used
	char *description;				// "BM25 with k1=<k1> and b=<b> [default k1=0.9 b=0.4, use 1.1:0.3 for INEX 2009]"
} ;


#endif /* RANKING_FUNCTION_FACTORY_OBJECT_H_ */
