/*
	INDEXER_PARAM_BLOCK.C
	---------------------
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "indexer_param_block.h"
#include "compression_text_factory.h"
#include "compression_factory.h"
#include "readability_factory.h"
#include "version.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_INDEXER_PARAM_BLOCK::ANT_INDEXER_PARAM_BLOCK()
	--------------------------------------------------
*/
ANT_indexer_param_block::ANT_indexer_param_block(int argc, char *argv[])
{
this->argc = argc;
this->argv = argv;
segmentation = ANT_parser::NOSEGMENTATION;
recursive = NONE;
compression_validation = FALSE;
compression_scheme = ANT_compression_factory::VARIABLE_BYTE;
readability_measure = ANT_readability_factory::NONE;

statistics = 0;
logo = TRUE;
reporting_frequency = LLONG_MAX;
ranking_function = IMPACT;
document_compression_scheme = NONE;
}

/*
	ANT_INDEXER_PARAM_BLOCK::USAGE()
	--------------------------------
*/
void ANT_indexer_param_block::usage(void)
{
printf("Usage:%s [option...] filename <... filename>\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ANT_INDEXER_PARAM_BLOCK::HELP()
	-------------------------------
*/
void ANT_indexer_param_block::help(void)
{
puts(ANT_version_string);
printf("Usage:%s [option...] filename <... filename>\n", argv[0]);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H        Display this help message");
puts("-nologo         Suppress banner");
puts("-people         Display credits");
puts("");

puts("FILE HANDLING");
puts("-------------");
puts("-r              Recursive search for files in this and directories below this");
puts("-rtbz2          Search in tar.bz2 files for indexable files");
puts("-rtrec          Single file, multiple <DOC>...</DOC> identified <DOCNO>docid</DOCNO>");
puts("-rtgz           Search in tar.gz files for indexable files");
puts("-rwarcgz        Search in warc.gz files for indexable files");
puts("-rzip           Search in .zip files for indexable files (PKZIP format files)");
puts("");

puts("COMPRESSION");
puts("-----------");
puts("-c[abBceEgnrsv] Compress postings using any of:");
puts("   a            try all schemes and choose the best  (same as -cceEgnrsv)");
puts("   b            try all bitwise schemes and choose the best  (same as -ceEg)");
puts("   B            try all Bytewise schemes and choose the best (same as -ccrsSv)");
puts("   c            Carryover-12  (bytewise)");
puts("   e            Elias Delta   (bitwise)");
puts("   E            Elias Gamma   (bitwise)");
puts("   g            Golomb        (bitwise)");
puts("   n            None          (-)");
puts("   r            Relative-10   (bytewise)");
puts("   s            Simple-9      (bytewise)");
puts("   S            Sigma         (bytewise)");
puts("   v            Variable Byte (bytewise) [default]");
puts("-vc             Validate posting compression (and report decompression rates)");
puts("-C[-bnz]        Store documents in the repository compressed with one of:");
puts("   -            don't create the repositorty [default]");
puts("   b            bz2");
puts("   n            not-compressed");
puts("   z            zip (deflate)");
puts("");

puts("SEGMENTATION");
puts("------------");
puts("-S[odb]        East-Asian word segmentation, Chinese segment is the text separated by non-Chinese");
puts("   o            Use a segmentation module to segment string on fly");
puts("   d            Double(dual) segmentation, with words and single characters");
puts("   b            Segmentation using bigram");
puts("");

puts("READABILITY");
puts("-----------");
puts("-R[ndf]         Calculate readability using one of:");
puts("   n            none [default]");
puts("   d            Dale-Chall");
puts("   f            Flesch-Kincaid");
puts("");

ANT_indexer_param_block_rank::help("QUANTIZATION", 'Q', index_functions);

puts("REPORTING");
puts("---------");
puts("-N<n>           Report time and memory every <n> documents [default -N0]");
puts("-s[-acmst]      Report statistics");
puts("   -            No statistics [default]");
puts("   a            All statistics");
puts("   c            Compression ratios");
puts("   m            Memory usage");
puts("   s            Summary statistics");
puts("   t            Timings");
puts("");

exit(0);
}

/*
	ANT_INDEXER_PARAM_BLOCK::DOCUMENT_COMPRESSION()
	-----------------------------------------------
*/
void ANT_indexer_param_block::document_compression(char *scheme)
{
switch (*scheme)
	{
	case '-': document_compression_scheme = NONE; break;
	case 'b': document_compression_scheme = ANT_compression_text_factory::BZ2; break;
	case 'n': document_compression_scheme = ANT_compression_text_factory::RAW; break;
	case 'z': document_compression_scheme = ANT_compression_text_factory::DEFLATE; break;
	default : exit(printf("Unknown compression scheme: '%c'\n", *scheme)); break;
	}

if (*(scheme + 1) != '\0')
	exit(printf("Only one document compresson scheme may be used at a time\n"));
}


/*
	ANT_INDEXER_PARAM_BLOCK::COMPRESSION()
	--------------------------------------
*/
void ANT_indexer_param_block::compression(char *scheme_list)
{
char *scheme;

for (scheme = scheme_list; *scheme != '\0'; scheme++)
	switch (*scheme)
		{
		case 'a': compression("ceEgnrsv"); break;
		case 'b': compression("eEg"); break;
		case 'B': compression("crsSv"); break;
		case 'c': compression_scheme |= ANT_compression_factory::CARRYOVER_12; break;
		case 'e': compression_scheme |= ANT_compression_factory::ELIAS_DELTA; break;
		case 'E': compression_scheme |= ANT_compression_factory::ELIAS_GAMMA; break;
		case 'g': compression_scheme |= ANT_compression_factory::GOLOMB; break;
		case 'n': compression_scheme |= ANT_compression_factory::NONE; break;
		case 'r': compression_scheme |= ANT_compression_factory::RELATIVE_10; break;
		case 's': compression_scheme |= ANT_compression_factory::SIMPLE_9; break;
		case 'S': compression_scheme |= ANT_compression_factory::SIGMA; break;
		case 'v': compression_scheme |= ANT_compression_factory::VARIABLE_BYTE; break;
		default : exit(printf("Unknown compression scheme: '%c'\n", *scheme)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::READABILITY()
	--------------------------------------
*/
void ANT_indexer_param_block::readability(char *measures)
{
char *measure;

for (measure = measures; *measure != '\0'; measure++)
	switch (*measure)
		{
		case 'n': readability_measure = ANT_readability_factory::NONE; break;
		case 'd': readability_measure |= ANT_readability_factory::DALE_CHALL; break;
		case 'f': readability_measure |= ANT_readability_factory::FLESCH_KINCAID; break;
		default : exit(printf("Unknown readability measure: '%c'\n", *measure)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::SEGMENTATION()
	--------------------------------------
*/
void ANT_indexer_param_block::segment(char *segment_flags)
{
char *segment_flag;

segmentation |= ANT_parser::SHOULD_SEGMENT;

for (segment_flag = segment_flags; *segment_flag != '\0'; segment_flag++)
	switch (*segment_flag)
		{
		//case 's': segmentation -= segmentation & ANT_parser::DOUBLE_SEGMENTATION; break; // only index what we have
		case 'o': segmentation |= ANT_parser::ONFLY_SEGMENTATION; break;
		case 'd': segmentation |= ANT_parser::DOUBLE_SEGMENTATION; break;
		case 'b': segmentation |= ANT_parser::BIGRAM_SEGMENTATION; break;
		default : exit(printf("Unknown segmentation flag: '%c'\n", *segment_flag)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::STATS()
	--------------------------------
*/
void ANT_indexer_param_block::stats(char *stat_list)
{
char *stat;

for (stat = stat_list; *stat != '\0'; stat++)
	switch (*stat)
		{
		case '-': statistics = 0;
		case 'a': stats("ccmst");
		case 'c': statistics |= STAT_COMPRESSION; break;
		case 'm': statistics |= STAT_MEMORY; break;
		case 's': statistics |= STAT_SUMMARY; break;
		case 't': statistics |= STAT_TIME; break;
		default : exit(printf("Unknown statistic: '%c'\n", *stat)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::PARSE()
	--------------------------------
*/
long ANT_indexer_param_block::parse(void)
{
long param;
char *command;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-'/* || *argv[param] == '/'*/)		// command line switch
		{
		command = argv[param] + 1;
		if (strcmp(command, "r") == 0)
			recursive = DIRECTORIES;
		else if (strcmp(command, "rtgz") == 0)
			recursive = TAR_GZ;
		else if (strcmp(command, "rtbz2") == 0)
			recursive = TAR_BZ2;
		else if (strcmp(command, "rzip") == 0)
			recursive = PKZIP;
		else if (strcmp(command, "rtrec") == 0)
			recursive = TREC;
		else if (strcmp(command, "rwarcgz") == 0)
			recursive = WARC_GZ;
		else if (*command == 'S')
			segment(command + 1);
		else if (strcmp(command, "?") == 0)
			help();
		else if (strcmp(command, "h") == 0)
			help();
		else if (strcmp(command, "H") == 0)
			help();
		else if (*command == 'N')
			{
			reporting_frequency = atol(command + 1);
			if (reporting_frequency == 0)
				reporting_frequency = LLONG_MAX;
			}
		else if (*command == 'C')
			{
			document_compression_scheme = NONE;
			document_compression(command + 1);
			}
		else if (*command == 'c')
			{
			compression_scheme = 0;
			compression(command + 1);
			}
		else if (strcmp(command, "vc") == 0)
			compression_validation = TRUE;
		else if (*command == 'R')
			{
			readability_measure = 0;
			readability(command + 1);
			}
		else if (*command == 's')
			{
			statistics = 0;
			stats(command + 1);
			}
		else if (*command == 'Q')
			set_ranker(command + 1);
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
		else if (strcmp(command, "people") == 0)
			{
			ANT_credits();
			exit(0);
			}
		else
			usage();
		}
	else
		break;
	}

return param;		// first parameter that isn't a command line switch
}
