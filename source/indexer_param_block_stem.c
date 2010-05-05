/*
	INDEXER_PARAM_BLOCK_STEM.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "stemmer_factory.h"
#include "indexer_param_block_stem.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_INDEXER_PARAM_BLOCK_STEM::ANT_INDEXER_PARAM_BLOCK_STEM()
	------------------------------------------------------------
*/
ANT_indexer_param_block_stem::ANT_indexer_param_block_stem()
{
stemmer = 0;
stemmer_similarity = FALSE;
stemmer_similarity_threshold = 0.0;
}

/*
	ANT_INDEXER_PARAM_BLOCK_STEM::HELP()
	------------------------------------
*/
void ANT_indexer_param_block_stem::help(long has_cutoff)
{
puts("TERM EXPANSION");
puts("--------------");
printf("-t[-hlops]");
if (has_cutoff)
	printf("[+-<th>] ");
else
	printf("    ");
printf("Term expansion, one of:\n");

puts("  -             None [default]");
#ifdef ANT_HAS_PAICE_HUSK
puts("  h             Paice Husk stemming");
#endif
#ifdef ANT_HAS_LOVINS
puts("  l             Lovins stemming");
#endif
puts("  o             Otago stemming");
puts("  p             Porter stemming");
puts("  s             S-Striping stemming");
if (has_cutoff)
	{
	#ifdef USE_FLOATED_TF
	puts("   +<th>        Stemmed terms tfs weighted by term similarity. [default=1]");
	#endif
	puts("   -<th>        Stemmed terms cutoff with term similarity. [default=0]");
	}
puts("");
}

/*
	ANT_INDEXER_PARAM_BLOCK_STEM::TERM_EXPANSION()
	----------------------------------------------
*/
void ANT_indexer_param_block_stem::term_expansion(char *which, long has_cutoff)
{
if (*(which + 1) != '\0' && *(which + 1) != '+' && *(which + 1) != '-')
	exit(printf("Only one term expansion algorithm is permitted: '%c'\n", *which));

switch (*which)
	{
	case '-' : stemmer = ANT_stemmer_factory::NONE;       break;
#ifdef ANT_HAS_PAICE_HUSK
	case 'h' : stemmer = ANT_stemmer_factory::PAICE_HUSK; break;
#endif
#ifdef ANT_HAS_LOVINS
	case 'l' : stemmer = ANT_stemmer_factory::LOVINS;     break;
#endif
	case 'p' : stemmer = ANT_stemmer_factory::PORTER;     break;
	case 'o' : stemmer = ANT_stemmer_factory::OTAGO;      break;
	case 's' : stemmer = ANT_stemmer_factory::S_STRIPPER; break;
	default : exit(printf("Unknown term expansion scheme: '%c'\n", *which)); break;
	}
if (!has_cutoff)
	if (*(which + 1) != '\0')
		exit(printf("Badly formed term expansion parameter:%s\n", which + 1));

if (*(which + 1) == '+')
#ifdef USE_FLOATED_TF
    {
    stemmer_similarity = ANT_stemmer_factory::WEIGHTED_SIMILARITY;
    if (*(which + 2) != '\0')
        stemmer_similarity_threshold = strtod(which + 2, NULL);
    else 
        stemmer_similarity_threshold = 1.0;
    }
#else
	exit(printf("Not compilied with floating point tf, option unsupported.\n"));
#endif
else if (*(which + 1) == '-')
    {
    stemmer_similarity = ANT_stemmer_factory::THRESHOLD_SIMILARITY;
    stemmer_similarity_threshold = strtod(which + 2, NULL);
    }
}
