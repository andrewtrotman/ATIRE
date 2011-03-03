/*
	INDEXER_PARAM_BLOCK_RANK.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "indexer_param_block_rank.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_dlh13.h"
#include "ranking_function_similarity.h"
#include "ranking_function_lmd.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"
#include "ranking_function_term_count.h"
#include "ranking_function_inner_product.h"
#include "ranking_function_kbtfidf.h"

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::ANT_INDEXER_PARAM_BLOCK_RANK()
	------------------------------------------------------------
*/
ANT_indexer_param_block_rank::ANT_indexer_param_block_rank()
{
ranking_function = BM25;
lmd_u = ANT_RANKING_FUNCTION_LMD_DEFAULT_U;
lmjm_l = ANT_RANKING_FUNCTION_LMJM_DEFAULT_LAMBDA;
bm25_k1 = ANT_RANKING_FUNCTION_BM25_DEFAULT_K1;
bm25_b = ANT_RANKING_FUNCTION_BM25_DEFAULT_B;

kbtfidf_k = ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_K;
kbtfidf_b = ANT_RANKING_FUNCTION_KBTFIDF_DEFAULT_B;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_TWO_PARAMETERS()
	--------------------------------------------------
*/
void ANT_indexer_param_block_rank::get_two_parameters(char *from, double *first, double *second)
{
char *ch;

for (ch = from; *ch != '\0'; ch++)
	if (*ch == ':')
		{
		*first = atof(ch + 1);
		break;
		}
	else
		puts("Command line parse error");

if (*ch != '\0')
	for (ch++; *ch != '\0'; ch++)
		if (*ch == ':')
			{
			*second = atof(ch + 1);
			break;
			}
		else if (!(isdigit(*ch) || *ch == '.'))
			puts("Command line parse error");

//printf("[%s][%f][%f]\n", from, *first, *second);
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_ONE_PARAMETER()
	-------------------------------------------------
*/
void ANT_indexer_param_block_rank::get_one_parameter(char *from, double *into)
{
char *ch;

for (ch = from; *ch != '\0'; ch++)
	if (*ch == ':')
		{
		*into = atof(ch + 1);
		break;
		}
	else
		puts("Command line parse error");

//printf("[%s][%f]\n", from, *into);
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::SET_RANKER()
	------------------------------------------
*/
void ANT_indexer_param_block_rank::set_ranker(char *which)
{
if (strncmp(which, "BM25", 4) == 0)
	{
	ranking_function = BM25;
	get_two_parameters(which + 4, &bm25_k1, &bm25_b);
	}
else if (strncmp(which, "lmd", 3) == 0)
	{
	ranking_function = LMD;
	get_one_parameter(which + 3, &lmd_u);
	}
else if (strncmp(which, "lmjm", 4) == 0)
	{
	ranking_function = LMJM;
	get_one_parameter(which + 4, &lmjm_l);
	}
else if (strcmp(which, "be") == 0)
	ranking_function = BOSE_EINSTEIN;
else if (strcmp(which, "divergence") == 0)
	ranking_function = DIVERGENCE;
else if (strcmp(which, "impact") == 0)
	ranking_function = IMPACT;
else if (strcmp(which, "DLH13") == 0)
	ranking_function = DLH13;
else if (strcmp(which, "readable") == 0)
	ranking_function = READABLE;
else if (strcmp(which, "termcount") == 0)
	ranking_function = TERM_COUNT;
else if (strcmp(which, "allterms") == 0)
	ranking_function = ALL_TERMS;
else if (strcmp(which, "tfidf") == 0)
	ranking_function = INNER_PRODUCT;
else if (strcmp(which, "kbtfidf") == 0)
	{
	ranking_function = KBTFIDF;
	get_two_parameters(which + 7, &kbtfidf_k, &kbtfidf_b);
	}
else
	exit(printf("Unknown Ranking Function:'%s'\n", which));
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
	printf("   DLH13        Terrier (claims to be ivergence from randomness using DLH13, but isn't) %s\n", isdefault(DIVERGENCE));
if (allowable & IMPACT)
	printf("   impact       Sum of impact scores %s\n", isdefault(IMPACT));
if (allowable & LMD)
	printf("   lmd:<u>      Language Models with Dirichlet smoothing, u=<u> [default u = 6750] %s\n" , isdefault(LMD));
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

puts("");
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_INDEXING_RANKER()
	---------------------------------------------------
*/
ANT_ranking_function *ANT_indexer_param_block_rank::get_indexing_ranker(long long documents, ANT_compressable_integer *lengths)
{
switch (ranking_function)
	{
	case BM25:
		return new ANT_ranking_function_BM25(documents, lengths, bm25_k1, bm25_b);
	case DLH13:
		return new ANT_ranking_function_DLH13(documents, lengths);
	case LMD:
		return new ANT_ranking_function_lmd(documents, lengths, lmd_u);
	case LMJM:
		return new ANT_ranking_function_lmd(documents, lengths, lmjm_l);
	case BOSE_EINSTEIN:
		return new ANT_ranking_function_bose_einstein(documents, lengths);
	case DIVERGENCE:
		return new ANT_ranking_function_divergence(documents, lengths);
	case TERM_COUNT:
		return new ANT_ranking_function_term_count(documents, lengths);
	case INNER_PRODUCT:
		return new ANT_ranking_function_inner_product(documents, lengths);
	case KBTFIDF:
		return new ANT_ranking_function_kbtfidf(documents, lengths, kbtfidf_k, kbtfidf_b);
	}
return NULL;
}
