/*
	ANT_PARAM_BLOCK.C
	-----------------
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#include "ant_param_block.h"
#include "version.h"
#include "stemmer_factory.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_ANT_PARAM_BLOCK::ANT_ANT_PARAM_BLOCK()
	------------------------------------------
*/
ANT_ANT_param_block::ANT_ANT_param_block(int argc, char *argv[])
{
this->argc = argc;
this->argv = argv;
logo = TRUE;
stemmer = 0;
sort_top_k = LLONG_MAX;
metric = MAP;
assessments_filename = NULL;
queries_filename = NULL;
output_forum = NONE;
run_name = participant_id = "unknown";
output_filename = "ant.out";
results_list_length = -1;
stats = SHORT;
segmentation = FALSE;
readability = FALSE;
thesaurus = FALSE;
thesaurus_threshold = 0.0;
}

/*
	ANT_ANT_PARAM_BLOCK::ANT_ANT_PARAM_BLOCK()
	------------------------------------------
*/
ANT_ANT_param_block::~ANT_ANT_param_block()
{
}

/*
	ANT_ANT_PARAM_BLOCK::USAGE()
	----------------------------
*/
void ANT_ANT_param_block::usage(void)
{
printf("Usage:%s [option...]\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ANT_ANT_PARAM_BLOCK::HELP()
	---------------------------
*/
void ANT_ANT_param_block::help(void)
{
puts(ANT_version_string);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H        Display this help message");
puts("-nologo         Suppress banner");
puts("");

puts("TERM EXPANSION");
puts("--------------");
puts("-t[-hlops]      Term expansion, one of:");
puts("  -             None [default]");
puts("  h             Paice Husk stemming");
puts("  l             Lovins stemming");
puts("  o             Otago stemming");
puts("  p             Porter stemming");
puts("  s             S-Striping stemming");
puts("");

puts("STEMMING SIMILARITY THRESHOLD");
puts("-----------");
puts("--termsim<n>    Prevent stemming if term similarity is <= n [default=0.0]");
puts("");

puts("OPTIMISATIONS");
puts("-------------");
puts("-k<n>           Results list accurate to the top <n> (0=all) [default=0]");
puts("");

puts("METRICS AND ASSESSMENTS");
puts("-----------------------");
puts("-m[metric]      Score the result set using");
puts("  MAP           Uninterpolated Mean Average Precision (TREC) [default]");
puts("  MAgP          Uninterpolated Mean Average generalised Precision (INEX)");
puts("  RankEff       Mean Rank Effectiveness (acount for unassessed documents)");
puts("-a<filenane>    Topic assessments are in <filename> (formats: ANT or INEX 2008)");
puts("-q<filename>    Queries are in file <filename> (format: ANT)");
puts("");

puts("TREC / INEX SPECIFIC");
puts("--------------------");
puts("-e[-it]         Export a run file for use in an Evaluation Forum");
puts("  -             Don't generate a run file [default]");
puts("  I             INEX 2008 (XML) run format");
puts("  i             INEX 2009 (TREC++) run format");
puts("  t             TREC run format");
puts("-o<filename>    Output filename for the run [default=ant.out]");
puts("-i<id>          Forum participant id is <id> [default=unknown]");
puts("-n<name>        Run is named <name> [default=unknown]");
puts("-l<n>           Length of the results list [default=1500 for batch, default=10 for interactive)]");
puts("");

puts("SEGMENTATION");
puts("------------");
puts("-S	           East-Asian language word segmentation");
puts("");

puts("READABILITY");
puts("-----------");
puts("-R              Use the readability search engine");
puts("");

puts("REPORTING");
puts("---------");
puts("-s[-aqQs]       Report statistics");
puts("   -            No statistics");
puts("   a            All statistics (same as -sqQs)");
puts("   q            Query by query statistics");
puts("   Q            Sum of query by query statistics for this run");
puts("   s            Short reporting (hits, average precision, etc) [default]");
puts("");


exit(0);
}

/*
	ANT_ANT_PARAM_BLOCK::SET_STATS()
	--------------------------------
*/
void ANT_ANT_param_block::set_stats(char *which)
{
do
	{
	switch (*which)
		{
		case '-' : stats = NONE;					break;
		case 'a' : stats |= QUERY | SUM | SHORT;	break;
		case 'q' : stats |= QUERY;					break;
		case 'Q' : stats |= SUM;    				break;
		case 's' : stats |= SHORT;    				break;
		default : exit(printf("Unknown stat: '%c'\n", *which)); break;
		}
	which++;
	}
while (*which != '\0');
}


/*
	ANT_ANT_PARAM_BLOCK::EXPORT_FORMAT()
	------------------------------------
*/
void ANT_ANT_param_block::export_format(char *forum)
{
do
	{
	switch (*forum)
		{
		case '-' : output_forum = NONE;   break;
		case 'I' : output_forum = INEX;   break;
		case 'i' : output_forum = TREC;   break;		// in 2009 INEX moved to the TREC format with extra stuff on the end of each line
		case 't' : output_forum = TREC;   break;
		default : exit(printf("Unknown export format: '%c'\n", *forum)); break;
		}
	forum++;
	}
while (*forum != '\0');
}

/*
	ANT_ANT_PARAM_BLOCK::SET_METRIC()
	---------------------------------
*/
void ANT_ANT_param_block::set_metric(char *which)
{
if (strcmp(which, "MAP") == 0)
	metric = MAP;
else if (strcmp(which, "MAgP") == 0)
	metric = MAgP;
else if (strcmp(which, "RankEff") == 0)
	metric = RANKEFF;
else
	exit(printf("Unknown metric:'%s'\n", which));
}

/*
	ANT_ANT_PARAM_BLOCK::TERM_EXPANSION()
	-------------------------------------
*/
void ANT_ANT_param_block::term_expansion(char *which)
{
if (*(which + 1) != '\0')
	exit(printf("Only one term expansion algorithm is permitted: '%c'\n", *which));

switch (*which)
	{
	case '-' : stemmer = ANT_stemmer_factory::NONE;       break;
	case 'h' : stemmer = ANT_stemmer_factory::PAICE_HUSK; break;
	case 'l' : stemmer = ANT_stemmer_factory::LOVINS;     break;
	case 'o' : stemmer = ANT_stemmer_factory::OTAGO;      break;
	case 'p' : stemmer = ANT_stemmer_factory::PORTER;     break;
	case 's' : stemmer = ANT_stemmer_factory::S_STRIPPER; break;
	default : exit(printf("Unknown term expansion scheme: '%c'\n", *which)); break;
	}
}

/*
	ANT_ANT_PARAM_BLOCK::PARSE()
	----------------------------
*/
long ANT_ANT_param_block::parse(void)
{
long param;
char *command;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-' || *argv[param] == '/')		// command line switch
		{
		command = argv[param] + 1;
		if (strcmp(command, "?") == 0)
			help();
                else if (strcmp(command, "h") == 0)
			help();
		else if (strcmp(command, "H") == 0)
			help();
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
		else if (*command == 't')
			term_expansion(command + 1);
		else if (*command == 'k')
			{
			sort_top_k = atol(command + 1);
			if (sort_top_k == 0)
				sort_top_k = LLONG_MAX;
			}
		else if (*command == 'm')
			set_metric(command + 1);
		else if (*command == 'a') 
            if (*(command + 1) == '\0' && param < argc - 1) 
                assessments_filename = argv[++param];
            else
                assessments_filename = command + 1;
		else if (*command == 'q')
            if (*(command + 1) == '\0' && param < argc - 1) 
                queries_filename = argv[++param];
            else
                queries_filename = command + 1;
		else if (*command == 'e')
			export_format(command + 1);
		else if (*command == 'i')
			participant_id = command + 1;
		else if (*command == 'n')
			run_name = command + 1;
		else if (*command == 'o')
			output_filename = command + 1;
		else if (*command == 'l')
			results_list_length = atol(command + 1);
		else if (*command == 's')
			set_stats(command + 1);
		else if (strcmp(command, "S") == 0)
			segmentation = TRUE;
		else if (*command == 'R')
			readability = TRUE;
		else if (strstr(command, "-termsim") == command) 
            {
            thesaurus = TRUE;
            thesaurus_threshold = strtod(command + 8, NULL);
            }
		else
			usage();
		}
	else
		break;
	}

/*
	If we're in batch mode (a query file has been specified) then the default
	length of the list of results is 1500.  If we're in interactive mode (no
	query file specified) then the default length is 10.
*/
if (results_list_length == -1)
	if (queries_filename == NULL)
		results_list_length = 10;
	else
		results_list_length = 1500;

return param;		// first parameter that isn't a command line switch
}

