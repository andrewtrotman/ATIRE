/*
	INDEXER_PARAM_BLOCK_RANK.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "str.h"
#include "indexer_param_block_rank.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_dlh13.h"
#include "ranking_function_dph.h"
#include "ranking_function_dfree.h"
#include "ranking_function_dfi.h"
#include "ranking_function_dfiw.h"
#include "ranking_function_dfi_idf.h"
#include "ranking_function_dfiw_idf.h"
#include "ranking_function_lmd.h"
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
	ANT_INDEXER_PARAM_BLOCK_RANK::!ANT_INDEXER_PARAM_BLOCK_RANK()
	------------------------------------------------------------_
*/
ANT_indexer_param_block_rank::~ANT_indexer_param_block_rank()
{
delete [] field_name;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::ANT_INDEXER_PARAM_BLOCK_RANK()
	------------------------------------------------------------
*/
ANT_indexer_param_block_rank::ANT_indexer_param_block_rank()
{
feedback_ranking_function = NONE;
ranking_function = BM25;

feedback_lmd_u = lmd_u = ANT_RANKING_FUNCTION_LMD_DEFAULT_U;
feedback_lmds_u = lmds_u = ANT_RANKING_FUNCTION_LMDS_DEFAULT_U;
feedback_lmjm_l = lmjm_l = ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA;
feedback_bm25_k1 = bm25_k1 = ANT_RANKING_FUNCTION_BM25_DEFAULT_K1;
feedback_bm25_b = bm25_b = ANT_RANKING_FUNCTION_BM25_DEFAULT_B;
feedback_kbtfidf_k = kbtfidf_k = ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_K;
feedback_kbtfidf_b = kbtfidf_b = ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_B;

ascending = 1;
field_name = NULL;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_TWO_PARAMETERS()
	--------------------------------------------------
	returns the number of parameters correctly parsed
*/
int ANT_indexer_param_block_rank::get_two_parameters(char *from, double *first, double *second)
{
char *ch = from;

if (*ch == ':')
	*first = atof(ch + 1);
else
	return 0;

for (ch++; *ch != '\0'; ch++)
	if (*ch == ':')
		{
		*second = atof(ch + 1);
		break;
		}
	else if (!(isdigit(*ch) || *ch == '.'))
		return 1;

return 2;
//printf("[%s][%f][%f]\n", from, *first, *second);
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_ONE_PARAMETER()
	-------------------------------------------------
*/
int ANT_indexer_param_block_rank::get_one_parameter(char *from, double *into)
{
if (*from == ':')
	{
	*into = atof(from + 1);
	return 1;
	}
else
	return 0;

//printf("[%s][%f]\n", from, *into);
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::SET_RANKER()
	------------------------------------------
	Parse the string to determine which ranking function (and parameters) to use.
	If the feedbacker parameter is true then set the ranking parameters for the feedbacker

	Returns non-zero on success, 0 on failure (e.g. no such ranking function or bad parameters).
	Failure leaves the parsed information in an undefined state.
*/
int ANT_indexer_param_block_rank::set_ranker(char *which, long feedbacker)
{
char *separator;
long ranking_function;

if (strncmp(which, "BM25", 4) == 0)
	{
	ranking_function = BM25;
	if (feedbacker)
		get_two_parameters(which + 4, &feedback_bm25_k1, &feedback_bm25_b);
	else
		get_two_parameters(which + 4, &bm25_k1, &bm25_b);
//	printf("K1=%f B=%f\n", bm25_k1, bm25_b);
	}
else if (strncmp(which, "lmds", 4) == 0)
	{
	ranking_function = LMDS;
	if (feedbacker)
		get_one_parameter(which + 4, &feedback_lmds_u);
	else
		get_one_parameter(which + 4, &lmds_u);
//	printf("U = %f\n", lmds_u);
	}
else if (strncmp(which, "lmd", 3) == 0)
	{
	ranking_function = LMD;
	if (feedbacker)
		get_one_parameter(which + 3, &feedback_lmd_u);
	else
		get_one_parameter(which + 3, &lmd_u);
//	printf("U = %f\n", lmd_u);
	}
else if (strncmp(which, "lmjm", 4) == 0)
	{
	ranking_function = LMJM;
	get_one_parameter(which + 4, &lmjm_l);
//	printf("L = %f\n", lmjm_l);
	}
else if (strcmp(which, "kbtfidf") == 0)
	{
	ranking_function = KBTFIDF;
	if (feedbacker)
		get_two_parameters(which + 7, &feedback_kbtfidf_k, &feedback_kbtfidf_b);
	else
		get_two_parameters(which + 7, &kbtfidf_k, &kbtfidf_b);
//	printf("K=%f B=%f\n", kbtfidf_k, kbtfidf_b);
	}
else if (strcmp(which, "be") == 0)
	ranking_function = BOSE_EINSTEIN;
else if (strcmp(which, "divergence") == 0)
	ranking_function = DIVERGENCE;
else if (strcmp(which, "impact") == 0)
	ranking_function = IMPACT;
else if (strcmp(which, "DLH13") == 0)
	ranking_function = DLH13;
else if (strcmp(which, "DPH") == 0)
	ranking_function = DPH;
else if (strcmp(which, "DFRee") == 0)
	ranking_function = DFREE;
else if (strcmp(which, "DFI") == 0)
	ranking_function = DFI;
else if (strcmp(which, "DFIW") == 0)
	ranking_function = DFIW;
else if (strcmp(which, "DFIIDF") == 0)
	ranking_function = DFI_IDF;
else if (strcmp(which, "DFIWIDF") == 0)
	ranking_function = DFIW_IDF;
else if (strcmp(which, "readable") == 0)
	ranking_function = READABLE;
else if (strcmp(which, "termcount") == 0)
	ranking_function = TERM_COUNT;
else if (strcmp(which, "allterms") == 0)
	ranking_function = ALL_TERMS;
else if (strcmp(which, "tfidf") == 0)
	ranking_function = INNER_PRODUCT;
else if (strcmp(which, "docida") == 0)
	{
	ranking_function = DOCID;
	ascending = 1;
	}
else if (strcmp(which, "docidd") == 0)
	{
	ranking_function = DOCID;
	ascending = 0;
	}
else if ((strncmp(which, "pregen:", strlen("pregen:")) == 0) && !feedbacker)
	{
	ranking_function = PREGEN;
	delete [] field_name;

	field_name = strnew(which + strlen("pregen:"));

	separator = strchr(field_name, ':');
	if (separator)
		{
		ascending = *(separator + 1) == 'a';
		*separator = '\0';
		}
	else
		ascending = 0;
	}
else
	return 0; //Unknown Ranking Function

if (feedbacker)
	this->feedback_ranking_function = ranking_function;
else
	this->ranking_function = ranking_function;

return 1;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::HELP()
	------------------------------------
*/
void ANT_indexer_param_block_rank::help(char *title, char switch_char, long long allowable)
{
printf("%s\n", title);
printf("%*.*s\n", (int)strlen(title), (int)strlen(title), "------------------------------------------");
printf("-%c[function]    Rank the result set using\n", switch_char);

if (allowable & BOSE_EINSTEIN)
	printf("   be           Bose-Einstein GL2%s\n", isdefault(BOSE_EINSTEIN));
if (allowable & BM25)
	printf("   BM25:<k1>:<b>BM25 with k1=<k1> and b=<b> [default k1=0.9 b=0.4, use 1.1:0.3 for INEX 2009] %s\n", isdefault(BM25));
if (allowable & DIVERGENCE)
	printf("   divergence   Divergence from randomness using I(ne)B2 %s\n", isdefault(DIVERGENCE));
if (allowable & DLH13)
	printf("   DLH13        Terrier DLH13 (Terrier's function, not the published DLH13) %s\n", isdefault(DLH13));
if (allowable & DPH)
	printf("   DPH          Terrier DPH %s\n", isdefault(DPH));
if (allowable & DFREE)
	printf("   DFRee        Terrier DFRee %s\n", isdefault(DFREE));
if (allowable & DFI)
	printf("   DFI          Divergence From Independence (TREC 2010 function)%s\n", isdefault(DFI));
if (allowable & DFIW)
	printf("   DFIW         Divergence From Independence Web (TREC 2011 function)%s\n", isdefault(DFIW));
if (allowable & DFI_IDF)
	printf("   DFI_IDF      Divergence From Independence with IDF %s\n", isdefault(DFI_IDF));
if (allowable & DFIW_IDF)
	printf("   DFIW_IDF     Divergence From Independence Web with IDF%s\n", isdefault(DFIW_IDF));
if (allowable & IMPACT)
	printf("   impact       Sum of impact scores %s\n", isdefault(IMPACT));
if (allowable & LMD)
	printf("   lmd:<u>      Language Models with Dirichlet smoothing, u=<u> [default u = 6750] %s\n" , isdefault(LMD));
if (allowable & LMDS)
	printf("   lmds:<u>     Language Models with Dirichlet smoothing (see Petri et al, ADCS 2013), u=<u> [default u = 2500] %s\n" , isdefault(LMDS));
if (allowable & LMJM)
	printf("   lmjm:<l>     Language Models with Jelinek-Mercer smoothing, l=<l> [default l = 0.5] %s\n", isdefault(LMJM));
if (allowable & READABLE)
	printf("   readable     The readability search engine (BM25 with Dale-Chall) %s\n", isdefault(READABLE));
if (allowable & TERM_COUNT)
	printf("   termcount    The number of query terms in the document (Boolean OR) %s\n", isdefault(TERM_COUNT));
if (allowable & ALL_TERMS)
	printf("   allterms     Relevant only if all query terms are present (Boolean AND) %s\n", isdefault(ALL_TERMS));
if (allowable & INNER_PRODUCT)
	printf("   tfidf        TF.IDF (vector space inner product) %s\n", isdefault(ALL_TERMS));
if (allowable & KBTFIDF)
	printf("   kbtfidf:<k>:<b> log(k * tf - b) * idf * idf (Shlomo's vector space) %s\n", isdefault(ALL_TERMS));
if (allowable & DOCID)
	printf("   docid<a|d>   Sort by document index (ascending or descending) %s\n", isdefault(DOCID));
if (allowable & PREGEN)
	printf("   pregen:<name>[:a] Pregenerated ranking, with field of given name, optionally ascending %s\n", isdefault(PREGEN));

puts("");
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_INDEXING_RANKER()
	---------------------------------------------------
*/
ANT_ranking_function *ANT_indexer_param_block_rank::get_indexing_ranker(long long documents, ANT_compressable_integer *lengths, long *quantization, long long *quantization_bits)
{
*quantization = this->quantization;
*quantization_bits = (long long)(this->quantization_bits == -1 ? 5.4 + 5.4e-4 * sqrt((double)documents) : this->quantization_bits);

switch (ranking_function)
	{
	case BM25:
		return new ANT_ranking_function_BM25(documents, lengths, *quantization_bits, bm25_k1, bm25_b);
	case DLH13:
		return new ANT_ranking_function_DLH13(documents, lengths, *quantization_bits);
	case DPH:
		return new ANT_ranking_function_DPH(documents, lengths, *quantization_bits);
	case DFREE:
		return new ANT_ranking_function_DFRee(documents, lengths, *quantization_bits);
	case DFI:
		return new ANT_ranking_function_DFI(documents, lengths, *quantization_bits);
	case DFIW:
		return new ANT_ranking_function_DFIW(documents, lengths, *quantization_bits);
	case DFI_IDF:
		return new ANT_ranking_function_DFI_IDF(documents, lengths, *quantization_bits);
	case DFIW_IDF:
		return new ANT_ranking_function_DFIW_IDF(documents, lengths, *quantization_bits);
	case LMD:
		return new ANT_ranking_function_lmd(documents, lengths, *quantization_bits, lmd_u);
	case LMDS:
		return new ANT_ranking_function_lmds(documents, lengths, *quantization_bits, lmds_u);
	case LMJM:
		return new ANT_ranking_function_lmd(documents, lengths, *quantization_bits, lmjm_l);
	case BOSE_EINSTEIN:
		return new ANT_ranking_function_bose_einstein(documents, lengths, *quantization_bits);
	case DIVERGENCE:
		return new ANT_ranking_function_divergence(documents, lengths, *quantization_bits);
	case TERM_COUNT:
		return new ANT_ranking_function_term_count(documents, lengths, *quantization_bits);
	case INNER_PRODUCT:
		return new ANT_ranking_function_inner_product(documents, lengths, *quantization_bits);
	case KBTFIDF:
		return new ANT_ranking_function_kbtfidf(documents, lengths, *quantization_bits, kbtfidf_k, kbtfidf_b);
	case DOCID:
		return new ANT_ranking_function_docid(documents, lengths, *quantization_bits, ascending);
	}
return NULL;
}

