/*
	RANKING_FUNCTION_FACTORY.C
	--------------------------
*/
#include "ranking_function_factory.h"
#include "ranking_function_factory_object.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_bm25l.h"
#include "ranking_function_bm25plus.h"
#include "ranking_function_bm25t.h"
#include "ranking_function_bm25adpt.h"
#include "ranking_function_tflodop.h"
#include "ranking_function_dlh13.h"
#include "ranking_function_dph.h"
#include "ranking_function_dfree.h"
#include "ranking_function_dfi.h"
#include "ranking_function_dfiw.h"
#include "ranking_function_dfi_idf.h"
#include "ranking_function_dfiw_idf.h"
#include "ranking_function_lmd.h"
#include "ranking_function_puurula.h"
#include "ranking_function_puurula_idf.h"
#include "ranking_function_lmds.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"
#include "ranking_function_term_count.h"
#include "ranking_function_inner_product.h"
#include "ranking_function_kbtfidf.h"
#include "ranking_function_docid.h"

/*
	ANT_LIST_OF_RANKERS
	-------------------
*/
ANT_ranking_function_factory_object ANT_list_of_rankers[] =
{
{ANT_ranking_function_factory_object::BM25,          "BM25",      "<k1>:<b>",     ANT_ranking_function_factory_object::INDEXABLE, 2, ANT_RANKING_FUNCTION_BM25_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25_DEFAULT_B, 0, ANT_RANKING_FUNCTION_BM25_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25_DEFAULT_B, 0, "BM25 with k1=<k1> and b=<b> [default k1=0.9 b=0.4, use 1.1:0.3 for INEX 2009"},
{ANT_ranking_function_factory_object::BM25L,         "BM25L",     "<k1>:<b>:<d>", ANT_ranking_function_factory_object::NONINDEXABLE, 3, ANT_RANKING_FUNCTION_BM25L_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25L_DEFAULT_B, ANT_RANKING_FUNCTION_BM25L_DEFAULT_DELTA, ANT_RANKING_FUNCTION_BM25L_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25L_DEFAULT_B, ANT_RANKING_FUNCTION_BM25L_DEFAULT_DELTA, "BM25L (Lv & Zhai) with k1=<k1>, b=<b>, delta=<d> [default k1=0.9 b=0.4, d=0.5]"},
{ANT_ranking_function_factory_object::BM25PLUS,      "BM25+",     "<k1>:<b>:<d>", ANT_ranking_function_factory_object::NONINDEXABLE, 3, ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_B, ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_DELTA, ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_B, ANT_RANKING_FUNCTION_BM25PLUS_DEFAULT_DELTA, "BM25+ (Lv & Zhai) with k1=<k1>, b=<b>, delta=<d> [default k1=0.9 b=0.4, d=1]"},
{ANT_ranking_function_factory_object::BM25T,         "BM25T",     "<b>",          ANT_ranking_function_factory_object::NONINDEXABLE, 1, ANT_RANKING_FUNCTION_BM25T_DEFAULT_B, 0, 0, ANT_RANKING_FUNCTION_BM25T_DEFAULT_B, 0, 0, "BM25T k1=log-logistic (Lv & Zhai) with b=<b> [default b=0.2]"},
{ANT_ranking_function_factory_object::BM25ADPT,      "BM25ADPT",  "<b>",          ANT_ranking_function_factory_object::NONINDEXABLE, 1, ANT_RANKING_FUNCTION_BM25ADPT_DEFAULT_B, 0, 0, ANT_RANKING_FUNCTION_BM25ADPT_DEFAULT_B, 0, 0, "BM25ADPT k1=information gain (Lv & Zhai) with b=<b> [default b=0.3]"},
{ANT_ranking_function_factory_object::TFLODOP,       "TFlodop",   "<b>:<d>",      ANT_ranking_function_factory_object::NONINDEXABLE, 2, ANT_RANKING_FUNCTION_TFLODOP_DEFAULT_B, ANT_RANKING_FUNCTION_TFLODOP_DEFAULT_DELTA, 0, ANT_RANKING_FUNCTION_TFLODOP_DEFAULT_B, ANT_RANKING_FUNCTION_TFLODOP_DEFAULT_DELTA, 0, "TFlodop*IDF (Rousseau & Vazirgiannis) with b=<b> and delta=<d> [default b=0.1, d=0.1]"},
{ANT_ranking_function_factory_object::LMD,           "lmd",       "<u>",          ANT_ranking_function_factory_object::NONINDEXABLE, 1, ANT_RANKING_FUNCTION_LMD_DEFAULT_U, 0, 0, ANT_RANKING_FUNCTION_LMD_DEFAULT_U, 0, 0, "Language Models with Dirichlet smoothing, u=<u> [default u = 6750]"},
{ANT_ranking_function_factory_object::LMDS,          "lmds",      "<u>",          ANT_ranking_function_factory_object::NONINDEXABLE, 1, ANT_RANKING_FUNCTION_LMDS_DEFAULT_U, 0, 0, ANT_RANKING_FUNCTION_LMDS_DEFAULT_U, 0, 0, "Language Models with Dirichlet smoothing (see Petri et al, ADCS 2013), u=<u> [default u = 2500]"},
{ANT_ranking_function_factory_object::PUURULA,       "lmp",       "<u>:<g>",      ANT_ranking_function_factory_object::NONINDEXABLE, 2, ANT_RANKING_FUNCTION_PUURULA_U, ANT_RANKING_FUNCTION_PUURULA_G, 0, ANT_RANKING_FUNCTION_PUURULA_U, ANT_RANKING_FUNCTION_PUURULA_G, 0, "Language Models with Pittman-Yor Process Dirichlet smoothing (see Puurula, ALTA 2013), u=<u>, g=<g> [default u=1000, g=0.5]"},
{ANT_ranking_function_factory_object::PUURULA_IDF,   "lmptfidf",  "<u>:<g>",      ANT_ranking_function_factory_object::NONINDEXABLE, 2, ANT_RANKING_FUNCTION_PUURULA_U, ANT_RANKING_FUNCTION_PUURULA_G, 0, ANT_RANKING_FUNCTION_PUURULA_U, ANT_RANKING_FUNCTION_PUURULA_G, 0, "LMP with TF.IDF weighting (see Puurula, ALTA 2013), u=<u>, g=<g> [default u=1000, g=0.5]"},
{ANT_ranking_function_factory_object::LMJM,          "lmjm",      "<l>",          ANT_ranking_function_factory_object::INDEXABLE, 1, ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA, 0, 0, ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA, 0, 0, "Language Models with Jelinek-Mercer smoothing, l=<l> [default l = 0.5]"},
{ANT_ranking_function_factory_object::DIVERGENCE,    "divergence", NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Divergence from randomness using I(ne)B2"},
{ANT_ranking_function_factory_object::BOSE_EINSTEIN, "be",         NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Bose-Einstein GL2"},
{ANT_ranking_function_factory_object::DLH13,         "DLH13",      NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Terrier DLH13 (Terrier's function, not the published DLH13)"},
{ANT_ranking_function_factory_object::DPH,           "DPH",        NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Terrier DPH"},
{ANT_ranking_function_factory_object::DFREE,         "DFRee",      NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Terrier DFRee"},
{ANT_ranking_function_factory_object::DFI,           "DFI",        NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Divergence From Independence (TREC 2010 function)"},
{ANT_ranking_function_factory_object::DFIW,          "DFIW",       NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Divergence From Independence Web (TREC 2011 function)"},
{ANT_ranking_function_factory_object::DFI_IDF,       "DFI_IDF",    NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Divergence From Independence with IDF)"},
{ANT_ranking_function_factory_object::DFIW_IDF,      "DFIW_IDF",   NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "Divergence From Independence Web with IDF"},
{ANT_ranking_function_factory_object::INNER_PRODUCT, "tfidf",      NULL,          ANT_ranking_function_factory_object::INDEXABLE, 0, 0, 0, 0, 0, 0, 0, "TF.IDF (vector space inner product)"},
{ANT_ranking_function_factory_object::KBTFIDF,       "kbtfidf",   "<k>:<b>",      ANT_ranking_function_factory_object::INDEXABLE, 2, ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_K, ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_B, 0, ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_K, ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_B, 0, "log(k * tf - b) * idf * idf (Shlomo's vector space)"},
{ANT_ranking_function_factory_object::ALL_TERMS,     "allterms",   NULL,          ANT_ranking_function_factory_object::NONINDEXABLE | ANT_ranking_function_factory_object::QUANTABLE, 0, 0, 0, 0, 0, 0, 0, "Relevant only if all query terms are present (Boolean AND)"},
{ANT_ranking_function_factory_object::TERM_COUNT,    "termcount",  NULL,          ANT_ranking_function_factory_object::INDEXABLE | ANT_ranking_function_factory_object::QUANTABLE, 0, 0, 0, 0, 0, 0, 0, "The number of query terms in the document (Boolean OR)"},
{ANT_ranking_function_factory_object::IMPACT,        "impact",     NULL,          ANT_ranking_function_factory_object::NONINDEXABLE | ANT_ranking_function_factory_object::QUANTABLE, 0, 0, 0, 0, 0, 0, 0, "Sum of impact scores"},
{ANT_ranking_function_factory_object::READABLE,      "readable",  "<k1>:<b>",     ANT_ranking_function_factory_object::NONINDEXABLE, 2, ANT_RANKING_FUNCTION_BM25_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25_DEFAULT_B, 0, ANT_RANKING_FUNCTION_BM25_DEFAULT_K1, ANT_RANKING_FUNCTION_BM25_DEFAULT_B, 0, "Readability (BM25 with Dale-Chall) [default k1=0.9 b=0.4]"},
{ANT_ranking_function_factory_object::DOCID,         "docid",     "<d>",          ANT_ranking_function_factory_object::NONINDEXABLE | ANT_ranking_function_factory_object::QUANTABLE, 1, 1, 0, 0, 1, 0, 0, "Sort by document index (<d=1> ascending or <d=0> descending [default d=1]"},
{ANT_ranking_function_factory_object::NONE,          "none"},       // sentinal every ranking function after here requires special handeling

{ANT_ranking_function_factory_object::PREGEN,        "pregen:<name>[:a]",    NULL,      ANT_ranking_function_factory_object::NONINDEXABLE | ANT_ranking_function_factory_object::QUANTABLE, 0, 0, 0, 0, 0, 0, 0, "Pregenerated ranking, with field of given name, optionally ascending"},
{ANT_ranking_function_factory_object::NONE,           NULL}        // sentinal end of list
};

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_INDEXING_RANKER()
	---------------------------------------------------
*/
ANT_ranking_function *ANT_ranking_function_factory::get_indexing_ranker(long long ranking_function, long long documents, ANT_compressable_integer *lengths, long long quantization_bits, double p1, double p2, double p3)
{
switch (ranking_function)
	{
	case ANT_ranking_function_factory_object::NONE:
		return NULL;
	case ANT_ranking_function_factory_object::BM25:
		return new ANT_ranking_function_BM25(documents, lengths, quantization_bits, p1, p2);
	case ANT_ranking_function_factory_object::LMJM:
		return new ANT_ranking_function_lmd(documents, lengths, quantization_bits, p1);
	case ANT_ranking_function_factory_object::DIVERGENCE:
		return new ANT_ranking_function_divergence(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::BOSE_EINSTEIN:
		return new ANT_ranking_function_bose_einstein(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DLH13:
		return new ANT_ranking_function_DLH13(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DPH:
		return new ANT_ranking_function_DPH(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DFREE:
		return new ANT_ranking_function_DFRee(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DFI:
		return new ANT_ranking_function_DFI(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DFIW:
		return new ANT_ranking_function_DFIW(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DFI_IDF:
		return new ANT_ranking_function_DFI_IDF(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DFIW_IDF:
		return new ANT_ranking_function_DFIW_IDF(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::INNER_PRODUCT:
		return new ANT_ranking_function_inner_product(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::KBTFIDF:
		return new ANT_ranking_function_kbtfidf(documents, lengths, quantization_bits, p1, p2);
	case ANT_ranking_function_factory_object::TERM_COUNT:
		return new ANT_ranking_function_term_count(documents, lengths, quantization_bits);
	case ANT_ranking_function_factory_object::DOCID:
		return new ANT_ranking_function_docid(documents, lengths, quantization_bits, (int)p1);
	default:
		printf("Error: Unknown ranking function selected in ANT_ranking_function_factory::get_indexing_ranker()\n");
	}
return NULL;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_FACTORY::GET_SEARCHING_RANKER()
	----------------------------------------------------
*/
ANT_ranking_function *ANT_ranking_function_factory::get_searching_ranker(ANT_search_engine *search_engine, long long function, long quantization, long long quantization_bits, double p1, double p2, double p3)
{
ANT_ranking_function *new_function;

switch (function)
	{
	case ANT_ranking_function_factory_object::BM25:
		new_function = new ANT_ranking_function_BM25(search_engine, quantization, quantization_bits, p1, p2);
		break;
	case ANT_ranking_function_factory_object::BM25L:
		new_function = new ANT_ranking_function_BM25L(search_engine, quantization, quantization_bits, p1, p2, p3);
		break;
	case ANT_ranking_function_factory_object::BM25PLUS:
		new_function = new ANT_ranking_function_BM25PLUS(search_engine, quantization, quantization_bits, p1, p2, p3);
		break;
	case ANT_ranking_function_factory_object::BM25T:
		new_function = new ANT_ranking_function_BM25T(search_engine, quantization, quantization_bits, p1);
		break;
	case ANT_ranking_function_factory_object::BM25ADPT:
		new_function = new ANT_ranking_function_BM25ADPT(search_engine, quantization, quantization_bits, p1);
		break;
	case ANT_ranking_function_factory_object::TFLODOP:
		new_function = new ANT_ranking_function_tflodop(search_engine, quantization, quantization_bits, p1, p2);
		break;
	case ANT_ranking_function_factory_object::LMD:
		new_function = new ANT_ranking_function_lmd(search_engine, quantization, quantization_bits, p1);
		break;
	case ANT_ranking_function_factory_object::LMDS:
		new_function = new ANT_ranking_function_lmds(search_engine, quantization, quantization_bits, p1);
		break;
	case ANT_ranking_function_factory_object::PUURULA:
		new_function = new ANT_ranking_function_puurula(search_engine, quantization, quantization_bits, p1, p2);
		break;
	case ANT_ranking_function_factory_object::PUURULA_IDF:
		new_function = new ANT_ranking_function_puurula_idf(search_engine, quantization, quantization_bits, p1, p2);
		break;
	case ANT_ranking_function_factory_object::LMJM:
		new_function = new ANT_ranking_function_lmjm(search_engine, quantization, quantization_bits, p1);
		break;
	case ANT_ranking_function_factory_object::DIVERGENCE:
		new_function = new ANT_ranking_function_divergence(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::BOSE_EINSTEIN:
		new_function = new ANT_ranking_function_bose_einstein(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DLH13:
		new_function = new ANT_ranking_function_DLH13(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DPH:
		new_function = new ANT_ranking_function_DPH(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DFREE:
		new_function = new ANT_ranking_function_DFRee(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DFI:
		new_function = new ANT_ranking_function_DFI(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DFIW:
		new_function = new ANT_ranking_function_DFIW(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DFI_IDF:
		new_function = new ANT_ranking_function_DFI_IDF(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DFIW_IDF:
		new_function = new ANT_ranking_function_DFIW_IDF(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::INNER_PRODUCT:
		new_function = new ANT_ranking_function_inner_product(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::KBTFIDF:
		new_function = new ANT_ranking_function_kbtfidf(search_engine, quantization, quantization_bits, p1, p2);
		break;
	case ANT_ranking_function_factory_object::ALL_TERMS:
		new_function = new ANT_ranking_function_term_count(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::TERM_COUNT:
		new_function = new ANT_ranking_function_term_count(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::IMPACT:
		new_function = new ANT_ranking_function_impact(search_engine, quantization, quantization_bits);
		break;
	case ANT_ranking_function_factory_object::DOCID:
		new_function = new ANT_ranking_function_docid(search_engine, quantization, quantization_bits, (int)p1);
		break;
	case ANT_ranking_function_factory_object::READABLE:
		/*
			I've got no idea why, but the readability function does not take quantization parameters.
			Someone should ask Matt Crane why not because he wrote both pieces of code.	
		*/
		new_function = new ANT_ranking_function_readability(search_engine, 0, 0, p1, p2);
		break;
	default:
		printf("Error: Unknown ranking function selected in ANT_ranking_function_factory::get_searching_ranker()\n");
		return NULL;		// failure, invalid parameter
	}

return new_function;
}