/*
	MERGER_PARAM_BLOCK.C
	--------------------
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "memory_index.h"
#include "maths.h"
#include "merger_param_block.h"
#include "compression_text_factory.h"
#include "compression_factory.h"
#include "version.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_MERGER_PARAM_BLOCK::ANT_MERGER_PARAM_BLOCK()
	-------------------------------------------------
*/
ANT_merger_param_block::ANT_merger_param_block(int argc, char *argv[]) : ANT_indexer_param_block(argc, argv)
{
this->argc = argc;
this->argv = argv;

reporting_frequency = 0;
index_filename = "merged_index.aspt";
doclist_filename = "merged_doclist.aspt";
document_compression_scheme = ANT_compression_text_factory::DEFLATE; // something non-NONE, doesn't matter what
}

/*
	ANT_MERGER_PARAM_BLOCK::USAGE()
	-------------------------------
*/
void ANT_merger_param_block::usage(void)
{
printf("Usage:%s [option...] <index 1> <index 2> <... index n>\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ANT_MERGER_PARAM_BLOCK::HELP()
	------------------------------
*/
void ANT_merger_param_block::help(void)
{
puts(ANT_version_string);
printf("Usage:%s [option...] <index 1> <index 2> <... index n>\n", argv[0]);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H        Display this help message");
puts("-nologo         Suppress banner");
puts("-people         Display credits");
puts("");

puts("OUPUT FILE HANDLING");
puts("--------------------");
puts("-findex <fn>    Output filename for index");
puts("-fdoclist <fn>  Output filename for doclist");
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
puts("-C[-]           Store documents in the repository compressed with one of:");
puts("   -            Don't create the repository");
puts("By default documents are included in the index if they are included in all indexes to be merged");
puts("and are compressed using the same methods that were used in the indexes to be merged.");
puts("If documents are not included in the index, then the doclists will have to be combined manually.");
puts("");

puts("OPTIMISATIONS");
puts("-------------");
puts("-K<n>           Static pruning. Write no more than <n> postings per list (0=all) [default=0]");
puts("                Static pruning will be performed to the minimum of the parameter given");
puts("                and the sum of static prune points in the indexes to be merged.");
puts("-k[-l0t][L<n>][s<n>] Term culling");
puts("   -            All terms remain in the indes [default]");
puts("   0            Do not index numbers");
puts("   l            Remove (stop) low frequency terms (where collection frequency == 1)");
puts("   L<n>         Remove (stop) low frequency terms (where document frequency <= <n>)");
puts("   s<n>         Remove (stop) words that occur in more than <n>% of documents");
puts("   S            Remove (stop) words that are on the NCBI PubMed MBR 313 word stopword list: wrd_stop");
puts("   t            Do not index XML tag names");
puts("");

exit(0);
}

/*
	ANT_MERGER_PARAM_BLOCK::PARSE()
	-------------------------------
*/
long ANT_merger_param_block::parse(void)
{
long param;
char *command;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-')		// command line switch
		{
		command = argv[param] + 1;
		if (strcmp(command, "?") == 0)
			help();
		else if (strcmp(command, "h") == 0)
			help();
		else if (strcmp(command, "H") == 0)
			help();
		else if (*command == 'k')
			term_removal(command + 1);
		else if (*command == 'K')
			{
			if ((static_prune_point = atoll(command + 1)) <= 0)
				static_prune_point = LLONG_MAX;
			}
		else if (*command == 'N')
			{
			reporting_frequency = atol(command + 1);
			if (reporting_frequency == 0)
				reporting_frequency = LLONG_MAX;
			}
		else if (*command == 'C')
			{
			if (*(command + 1) != '-')
				exit(printf("Unsupported document compression.\n"));
			document_compression_scheme = NONE;
			}
		else if (*command == 'c')
			{
			compression_scheme = 0;
			compression(command + 1);
			}
		else if (strcmp(command, "findex") == 0)
			index_filename = argv[++param];
		else if (strcmp(command, "fdoclist") == 0)
			doclist_filename = argv[++param];
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
