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
#include "ranking_function_similarity.h"
#include "ranking_function_lmd.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::ANT_INDEXER_PARAM_BLOCK_RANK()
	------------------------------------------------------------
*/
ANT_indexer_param_block_rank::ANT_indexer_param_block_rank()
{
ranking_function = BM25;
lmd_u = 500.0;
lmjm_l = 0.5;
bm25_k1 = 0.9;
bm25_b = 0.4;
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
else if (strcmp(which, "readable") == 0)
	ranking_function = READABLE;
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
printf("%*.*s\n", strlen(title), strlen(title), "------------------------------------------");
printf("-%c[function]    Rank the result set using\n", switch_char);

if (allowable & BOSE_EINSTEIN)
	printf("   be           Bose-Einstein %s\n", isdefault(BOSE_EINSTEIN));
if (allowable & BM25)
	printf("   BM25:<k1>:<b>BM25 with k1=<k1> and b=<b> [default k1=0.9 b=0.4] %s\n", isdefault(BM25));
if (allowable & DIVERGENCE)
	printf("   divergence   Divergence from randomness using I(ne)B2 %s\n", isdefault(DIVERGENCE));
if (allowable & IMPACT)
	printf("   impact       Sum of impact scores %s\n", isdefault(IMPACT));
if (allowable & LMD)
	printf("   lmd:<u>      Language Models with Dirichlet smoothing, u=<u> [default u=500] %s\n" , isdefault(LMD));
if (allowable & LMJM)
	printf("   lmjm:<l>     Langyage Models with Jelinek-Mercer smoothing, l=<n> [default l=0.1] %s\n", isdefault(LMJM));
if (allowable & READABLE)
	printf("   readable     The readability search engine (BM25 with Dale-Chall) %s\n", isdefault(READABLE));
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
	case LMD:
		return new ANT_ranking_function_lmd(documents, lengths, lmd_u);
	case LMJM:
		return new ANT_ranking_function_lmd(documents, lengths, lmjm_l);
	case BOSE_EINSTEIN:
		return new ANT_ranking_function_bose_einstein(documents, lengths);
	case DIVERGENCE:
		return new ANT_ranking_function_divergence(documents, lengths);
	}
return NULL;
}
