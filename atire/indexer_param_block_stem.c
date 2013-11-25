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
char paice_husk[2], snowball[2];

paice_husk[0] = paice_husk[1] = snowball[0] = snowball[1] = '\0';

#ifdef ANT_HAS_PAICE_HUSK
	paice_husk[0] = 'h';
#endif
#ifdef ANT_HAS_SNOWBALL
	snowball[0] = 'X';
#endif

puts("TERM EXPANSION");
puts("--------------");
printf("-t[-D%skoOpsS%s]", paice_husk, snowball);

if (has_cutoff)
	printf("[+-<th>] ");
else
	printf("  ");
printf("Term expansion, one of:\n");

puts("  -             None [default]");
puts("  D             Double Metaphone phonetics");
#ifdef ANT_HAS_PAICE_HUSK
puts("  h             English Paice Husk stemming");
#endif
puts("  k             English Krovetz stemming");
puts("  o             English Otago stemming");
puts("  O             English Otago stemming version 2");
puts("  p             English Porter stemming (ATIRE's version of Porter v1)");
puts("  s             English S-Stripping stemming");
puts("  S             Soundex phonetics");
#ifdef ANT_HAS_SNOWBALL
	puts("  X[abcDdEeFfg2hiklnpRrSst] Stemming using the Snowball libraries");
	puts("   a Armenian");
	puts("   b Basque");
	puts("   c Catalan");
	puts("   D Danish");
	puts("   d Dutch");
	puts("   k Dutch (using the Snowball variant of Kraaij Pohlmann)");
	puts("   E English (using the Porter stemmer verison 1)");
	puts("   e English (using the Porter stemmer version 2)");
	puts("   l English (using the Lovins stemmer)");
	puts("   F Finnish");
	puts("   f French");
	puts("   g German (version 1)");
	puts("   2 German (version 2)");
	puts("   h Hungarian");
	puts("   i Italian");
	puts("   n Norwegian");
	puts("   p Portuguese");
	puts("   R Romanian");
	puts("   r Russian");
	puts("   S Spanish");
	puts("   s Swedish");
	puts("   t Turkish");
#endif

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
if (*(which + 1) != '\0' && *(which + 1) != '+' && *(which + 1) != '-' && (*which != 'X'))
	exit(printf("Only one term expansion algorithm is permitted: '%c'\n", *which));

switch (*which)
	{
	case '-' : stemmer = ANT_stemmer_factory::NONE;       break;
#ifdef ANT_HAS_PAICE_HUSK
	case 'h' : stemmer = ANT_stemmer_factory::PAICE_HUSK; break;
#endif
	case 'k' : stemmer = ANT_stemmer_factory::KROVETZ;    break;
	case 'p' : stemmer = ANT_stemmer_factory::PORTER;     break;
	case 'o' : stemmer = ANT_stemmer_factory::OTAGO;      break;
	case 'O' : stemmer = ANT_stemmer_factory::OTAGO_V2;   break;
	case 's' : stemmer = ANT_stemmer_factory::S_STRIPPER; break;
	case 'D' : stemmer = ANT_stemmer_factory::DOUBLE_METAPHONE;       break;
	case 'S' : stemmer = ANT_stemmer_factory::SOUNDEX; break;
	case 'X' :
		if (*(which + 2) != '\0')
			exit(printf("Unknown stemmer: '-%c'\n", *which));

		switch (*(which + 1))
			{
			case 'a': stemmer = ANT_stemmer_factory::SNOWBALL_ARMENIAN; break;
			case 'b': stemmer = ANT_stemmer_factory::SNOWBALL_BASQUE; break;
			case 'c': stemmer = ANT_stemmer_factory::SNOWBALL_CATALAN; break;
			case 'D': stemmer = ANT_stemmer_factory::SNOWBALL_DANISH; break;
			case 'd': stemmer = ANT_stemmer_factory::SNOWBALL_DUTCH; break;
			case 'E': stemmer = ANT_stemmer_factory::SNOWBALL_PORTER; break;
			case 'e': stemmer = ANT_stemmer_factory::SNOWBALL_ENGLISH; break;
			case 'F': stemmer = ANT_stemmer_factory::SNOWBALL_FINNISH; break;
			case 'f': stemmer = ANT_stemmer_factory::SNOWBALL_FRENCH; break;
			case 'g': stemmer = ANT_stemmer_factory::SNOWBALL_GERMAN; break;
			case '2': stemmer = ANT_stemmer_factory::SNOWBALL_GERMAN2; break;
			case 'h': stemmer = ANT_stemmer_factory::SNOWBALL_HUNGARIAN; break;
			case 'i': stemmer = ANT_stemmer_factory::SNOWBALL_ITALIAN; break;
			case 'l': stemmer = ANT_stemmer_factory::SNOWBALL_LOVINS; break;
			case 'k': stemmer = ANT_stemmer_factory::SNOWBALL_KRAAIJ_POHLMANN; break;
			case 'n': stemmer = ANT_stemmer_factory::SNOWBALL_NORWEGIAN; break;
			case 'p': stemmer = ANT_stemmer_factory::SNOWBALL_PORTUGUESE; break;
			case 'R': stemmer = ANT_stemmer_factory::SNOWBALL_ROMANIAN; break;
			case 'r': stemmer = ANT_stemmer_factory::SNOWBALL_RUSSIAN; break;
			case 'S': stemmer = ANT_stemmer_factory::SNOWBALL_SPANISH; break;
			case 's': stemmer = ANT_stemmer_factory::SNOWBALL_SWEDISH; break;
			case 't': stemmer = ANT_stemmer_factory::SNOWBALL_TURKISH; break;
			default : exit(printf("Unknown Snowball term expansion scheme: 'X%c'\n", *(which + 1))); break;
			}
		break;
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
