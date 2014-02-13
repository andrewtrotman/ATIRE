/*
	INDEXER_PARAM_BLOCK_TOPSIG.C
	----------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "indexer_param_block_topsig.h"
#include "memory_index.h"

/*
	ANT_INDEXER_PARAM_BLOCK_TOPSIG::ANT_INDEXER_PARAM_BLOCK_TOPSIG()
	----------------------------------------------------------------
*/
ANT_indexer_param_block_topsig::ANT_indexer_param_block_topsig()
{
inversion_type = INVERTED_FILE;
topsig_width = 4096;				// width of the signature (in bits)
topsig_density = 12.0;				// 12% of bits are +ve and 12% are -ve in each terms
topsig_global_stats = NULL;
inversion_extras = ANT_memory_index::NONE;
}

/*
	ANT_INDEXER_PARAM_BLOCK_TOPSIG::TOPSIG()
	----------------------------------------
*/
void ANT_indexer_param_block_topsig::topsig(char *params)
{
char *colon;

inversion_type = TOPSIG;
topsig_width = atol(params);
if ((colon = strchr(params, ':')) != NULL)
	{
	colon++;
	topsig_density = atof(colon);
	if ((colon = strchr(colon, ':')) != NULL)
		topsig_global_stats = colon + 1;
	}
}

/*
	ANT_INDEXER_PARAM_BLOCK_TOPSIG::HELP()
	--------------------------------------
*/
void ANT_indexer_param_block_topsig::help(void)
{
puts("INDEX FILE TYPE");
puts("---------------");
puts("-Topsig:<w>:<d>:<f> Generate a TopSig index of width <w>bits density <d>% and globalstats <f>");
puts("-Inverted       Generate an inverted file [default]"); 
puts("-Ilmp:<g>       Inverted file with Puurula discounted length vectors [default g=0.9124]"); 
puts("-Ilmptfidf:<g>    Inverted file with Puurula discounted length vectors using tf.idf[default g=0.9124]");
puts("");
}