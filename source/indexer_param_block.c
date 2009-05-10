/*
	INDEXER_PARAM_BLOCK.C
	---------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "indexer_param_block.h"
#include "compression_factory.h"
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
trec_docnos = recursive = FALSE;
compression_validation = FALSE;
compression_scheme = ANT_compression_factory::VARIABLE_BYTE;
}

/*
	ANT_INDEXER_PARAM_BLOCK::HELP()
	-------------------------------
*/
void ANT_indexer_param_block::help(void)
{
puts(ANT_version_string);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H  Display this help message");
puts("");

puts("FILE HANDLING");
puts("-------------");
puts("-r        Recursive search for files in this and directories below this");
puts("");

puts("INDEXING");
puts("--------");
puts("-c[bBceEgnrsv] Compress postings using one of:");
puts("   b try all bitwise schemes and choose the best  (same as -ceEg)");
puts("   B try all Bytewise schemes and choose the best (same as -ccrsSv)");
puts("   c Carryover-12  (bytewise)");
puts("   e Elias Delta   (bitwise)");
puts("   E Elias Gamma   (bitwise)");
puts("   g Golomb        (bitwise)");
puts("   n None          (-)");
puts("   r Relative-10   (bytewise)");
puts("   s Simple-9      (bytewise)");
puts("   S Sigma         (bytewise)");
puts("   v Variable Byte (bytewise) [default]");
puts("");

puts("TREC / INEX SPECIFIC");
puts("--------------------");
puts("-docno  <DOC>...</DOC> seperates documents (identified by <DOCNO>docid</DOCNO>)");
puts("        default is each doc in seperate file and docid is filename");
puts("-trec   see -docno");
puts("");

puts("OTHER");
puts("-----");
puts("-vc     Validate compression (and report decompression rate)");
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
	ANT_INDEXER_PARAM_BLOCK::PARSE()
	--------------------------------
*/
long ANT_indexer_param_block::parse(void)
{
long param;
char *command;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-' || *argv[param] == '/')		// command line switch
		{
		command = argv[param] + 1;
		if (strcmp(command, "docno") == 0)
			trec_docnos = TRUE;
		else if (strcmp(command, "trec") == 0)
			trec_docnos = TRUE;
		else if (strcmp(command, "r") == 0)
			recursive = TRUE;
		else if (strcmp(command, "?") == 0)
			help();
		else if (strcmp(command, "h") == 0)
			help();
		else if (strcmp(command, "H") == 0)
			help();
		else if (*command == 'c')
			{
			compression_scheme = 0;
			compression(command + 1);
			}
		else if (strcmp(command, "vc") == 0)
			compression_validation = TRUE;
		else
			help();
		}
	else
		break;
	}

return param;		// first parameter that isn't a command line switch
}

