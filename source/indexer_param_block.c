/*
	INDEXER_PARAM_BLOCK.C
	---------------------
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "indexer_param_block.h"
#include "compression_factory.h"
#include "encoding_factory.h"
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
trec_docnos = recursive = segmentation = FALSE;
compression_validation = FALSE;
compression_scheme = ANT_compression_factory::VARIABLE_BYTE;
readability_measure = ANT_readability_factory::NONE;
statistics = 0;
logo = TRUE;
reporting_frequency = LLONG_MAX;
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
puts("");

puts("FILE HANDLING");
puts("-------------");
puts("-r              Recursive search for files in this and directories below this");
puts("");

puts("TREC / INEX SPECIFIC");
puts("--------------------");
puts("-docno          <DOC>...</DOC> seperates documents (identified by <DOCNO>docid</DOCNO>)");
puts("                default is each doc in seperate file and docid is filename");
puts("-trec           see -docno");
puts("");

puts("COMPRESSION");
puts("-----------");
puts("-c[abBceEgnrsv] Compress postings using one of:");
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
puts("-vc             Validate compression (and report decompression rates)");
puts("");

puts("ENCODING");
puts("-----------");
puts("-e[u] decode the input text using one of encodings:");
puts("   u            UTF8 encoding");
puts("");

puts("SEGMENTATION(EAST-ASIAN LANGUAGES ONLY)");
puts("-----------");
puts("-S	          segment the text into meaningful words");
puts("");

puts("READABILITY");
puts("-----------");
puts("-R[nd]          Calculate a readability measure using one of:");
puts("   n            none [default]");
puts("   d            Dale-Chall");
puts("");

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
	ANT_INDEXER_PARAM_BLOCK::ENCODING()
	-----------------------------------
*/
void ANT_indexer_param_block::encoding(char *scheme_list)
{
char *scheme;

for (scheme = scheme_list; *scheme != '\0'; scheme++)
	switch (*scheme)
		{
		case 'u': encoding_scheme = ANT_encoding_factory::UTF8; break;
		default : exit(printf("Unknown encoding scheme: '%c'\n", *scheme)); break;
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
		case 'd': readability_measure = ANT_readability_factory::DALE_CHALL; break;
		default : exit(printf("Unknown readability measure: '%c'\n", *measure)); break;
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
		if (strcmp(command, "docno") == 0)
			trec_docnos = TRUE;
		else if (strcmp(command, "trec") == 0)
			trec_docnos = TRUE;
		else if (strcmp(command, "r") == 0)
			recursive = TRUE;
		else if (strcmp(command, "S") == 0)
			segmentation = TRUE;
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
		else if (*command == 'c')
			{
			compression_scheme = 0;
			compression(command + 1);
			}
		else if (*command == 'e')
			{
			encoding_scheme = 0;
			encoding(command + 1);
			}
		else if (strcmp(command, "vc") == 0)
			compression_validation = TRUE;
		else if (*command == 's')
			{
			statistics = 0;
			stats(command + 1);
			}
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
		else
			usage();
		}
	else
		break;
	}

return param;		// first parameter that isn't a command line switch
}
