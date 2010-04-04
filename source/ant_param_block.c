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
port = 0;
this->argc = argc;
this->argv = argv;
logo = TRUE;
stemmer = 0;
stemmer_similarity = FALSE;
stemmer_similarity_threshold = 0.0;
sort_top_k = LLONG_MAX;
focus_top_k = 2000;
metric = MAP;
metric_n = 10;
assessments_filename = NULL;
queries_filename = NULL;
output_forum = NONE;
run_name = participant_id = "unknown";
output_filename = "ant.out";
results_list_length = -1;
stats = SHORT;
segmentation = TRUE;
trim_postings_k = LLONG_MAX;
file_or_memory = INDEX_IN_FILE;
focussing_algorithm = NONE;
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
puts("-people         Display credits");
puts("");

puts("TERM EXPANSION");
puts("--------------");
puts("-t[-hlops][+-<th>]  Term expansion, one of:");
puts("  -             None [default]");
puts("  h             Paice Husk stemming");
puts("  l             Lovins stemming");
puts("  o             Otago stemming");
puts("  p             Porter stemming");
puts("  s             S-Striping stemming");
#ifdef USE_FLOATED_TF
puts("   +<th>        Stemmed terms tfs weighted by term similarity. [default=1]");
#endif
puts("   -<th>        Stemmed terms cutoff with term similarity. [default=0]");
puts("");

puts("OPTIMISATIONS");
puts("-------------");
puts("-k<n>           Top-k search. Document results list accurate to the top <n> (0=all) [default=0]");
puts("-F<n>           Focus-k. Focused results list accurate to the top <n> [default=2000]");
puts("-K<n>           Static pruning. Process no fewer than <n> postings (0=all) [default=0]");
puts("-M              Load the index into memory at startup");
puts("");

puts("METRICS AND ASSESSMENTS");
puts("-----------------------");
puts("-m[metric]      Score the result set using");
puts("  MAP           Documents, Uninterpolated Mean Average Precision (TREC) [default]");
puts("  MAgP          Documents, Uninterpolated Mean Average generalised Precision (INEX)");
//puts("  MAgPf         Passages, Uninterpolated Mean Average generalised Precision (INEX)");
//puts("  MAiPf         Passages, Measn Average interpolated Precision (INEX)");
puts("  P@<n>         Documents, Set-based precision at <n> [default=10]");
puts("  S@<n>         Documents, Set-based success (1=found at least 1 relevant or 0=none) at <n> [default=10]");
puts("  RankEff       Documents, Mean Rank Effectiveness (acount for unassessed documents)");
puts("-a<filenane>    Topic assessments are in <filename> (formats: ANT or INEX 2008)");
puts("-q<filename>    Queries are in file <filename> (format: ANT)");
puts("-q:<port>       ANT SERVER:Queries from TCP/IP port <port> [default=8088]");
puts("");

puts("TREC / INEX SPECIFIC");
puts("--------------------");
puts("-e[-Iifbet]     Export a run file for use in an Evaluation Forum");
puts("  -             Don't generate a run file [default]");
puts("  I             INEX 2008 (XML) run format");
puts("  i             INEX 2009 (TREC++) run format (documents only)");
puts("  f             INEX 2009 (TREC++) run format (documents and passages)");
puts("  b             INEX 2009 (TREC++) run format (best entry point)");
puts("  e             INEX 2009 Efficiency (TREC++) run format");
puts("  t             TREC run format");
puts("-o<filename>    Output filename for the run [default=ant.out]");
puts("-i<id>          Forum participant id is <id> [default=unknown]");
puts("-n<name>        Run is named <name> [default=unknown]");
puts("-l<n>           Length of the results list [default=1500 for batch, default=10 for interactive)]");
puts("");

puts("SEGMENTATION");
puts("------------");
puts("-S[n]           East-Asian language word segmentation, query is segmented into characters by default");
puts("  n             No segmentation, search with the input terms separated by space");
puts("");

ANT_indexer_param_block_rank::help("RANKING", 'R', search_functions);		// ranking dunctions

puts("FOCUSED RETRIEVAL");
puts("-----------------");
puts("-f[ar]          Focus the results list");
puts("  a             ARTICLE: Article retrieval [default]");
puts("  r             RANGE: Start tag before the first occurence to end tag after last");
puts("");

puts("REPORTING");
puts("---------");
puts("-s[-aqQs]       Report statistics");
puts("  -             No statistics");
puts("  a             All statistics (same as -sqQs)");
puts("  q             Query by query statistics");
puts("  Q             Sum of query by query statistics for this run");
puts("  s             Short reporting (hits, average precision, etc) [default]");

exit(0);
}

/*
	ANT_ANT_PARAM_BLOCK::SET_STATS()
	--------------------------------
*/
void ANT_ANT_param_block::set_stats(char *which)
{
stats = 0;
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
		focussing_algorithm = ARTICLE;				// if we're not focusing yet then we are now!
*/
void ANT_ANT_param_block::export_format(char *forum)
{
do
	{
	switch (*forum)
		{
		case '-' : output_forum = NONE;   break;
		case 'I' : output_forum = INEX;   break;
		case 'e' : output_forum = INEX_EFFICIENCY; break;
		case 'i' : output_forum = TREC;   break;			// in 2009 INEX moved to the TREC format with extra stuff on the end of each line
		case 'b' : output_forum = INEX_BEP;   break;		// the INEX 2009 format with best entry points
		case 't' : output_forum = TREC;   break;
		case 'f' : 											// the INEX 2009 format with focused results included
			output_forum = INEX_FOCUS;
			if (focussing_algorithm == NONE)
				focussing_algorithm = ARTICLE;				// if we're not focusing yet then we are now!
			break;
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
else if (strcmp(which, "MAgPf") == 0)
	{
	metric = MAgPf;
	if (focussing_algorithm == NONE)
		focussing_algorithm = ARTICLE;				// if we're not focusing yet then we are now!
	}
else if (strcmp(which, "MAiP") == 0)
	{
	metric = MAiP;
	if (focussing_algorithm == NONE)
		focussing_algorithm = ARTICLE;				// if we're not focusing yet then we are now!
	}
else if (strcmp(which, "RankEff") == 0)
	metric = RANKEFF;
else if (strncmp(which, "P@", 2) == 0)
	{
	metric = P_AT_N;
	if (ANT_isdigit(which[2]))
		{
		metric_n = atol(which + 2);
		if (metric_n == 0)
			exit(printf("Nice Try... You can't compute P@0!\n"));
		}
	else
		exit(printf("<n> in P@<n> must be numeric (e.g. P@10)"));
	}
else if (strncmp(which, "S@", 2) == 0)
	{
	metric = SUCCESS_AT_N;
	if (ANT_isdigit(which[2]))
		{
		metric_n = atol(which + 2);
		if (metric_n == 0)
			exit(printf("Nice Try... You can't compute S@0!\n"));
		}
	else
		exit(printf("<n> in S@<n> must be numeric (e.g. S@10)"));
	}
else
	exit(printf("Unknown metric:'%s'\n", which));
}

/*
	ANT_ANT_PARAM_BLOCK::TERM_EXPANSION()
	-------------------------------------
*/
void ANT_ANT_param_block::term_expansion(char *which)
{
if (*(which + 1) != '\0' && *(which + 1) != '+' && *(which + 1) != '-')
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

/*
	ANT_ANT_PARAM_BLOCK::SET_FOCUSED_RANKER()
	-----------------------------------------
*/
void ANT_ANT_param_block::set_focused_ranker(char *which)
{
if (*(which + 1) != '\0')
	exit(printf("Only one focusing algorithm is permitted\n"));

switch (*which)
	{
	case '-' : focussing_algorithm = NONE; break;
	case 'a' : focussing_algorithm = ARTICLE; break;
	case 'r' : focussing_algorithm = RANGE; break;
	default : exit(printf("Unknown focusing algorithm: '%c'\n", *which)); break;
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
		else if (*command == 'K')
			{
			trim_postings_k = atol(command + 1);
			if (trim_postings_k == 0)
				trim_postings_k = LLONG_MAX;
			}
		else if (*command == 'F')
			focus_top_k = atol(command + 1);
		else if (*command == 'M')
			file_or_memory = INDEX_IN_MEMORY;
		else if (*command == 'm')
			set_metric(command + 1);
		else if (*command == 'a') 
			if (*(command + 1) == '\0' && param < argc - 1) 
				assessments_filename = argv[++param];
			else
				assessments_filename = command + 1;
		else if (*command == 'q')
			{
			if (command[1] == ':')
				port = (unsigned short)(isdigit(command[2]) ? atol(command + 2) : 8088);
			else
				{
				if (*(command + 1) == '\0' && param < argc - 1) 
					queries_filename = argv[++param];
				else
					queries_filename = command + 1;
				}
			}
		else if (*command == 'e')
			export_format(command + 1);
		else if (*command == 'i')
			participant_id = command + 1;
		else if (*command == 'n')
			run_name = command + 1;
		else if (*command == 'o')
			{
			if (*(command + 1) != '\0')
				output_filename = command + 1;
			}
		else if (*command == 'l')
			results_list_length = atol(command + 1);
		else if (*command == 's')
			set_stats(command + 1);
		else if (*command == 'S')
			{
			++command;
			if (*command == 'n')
				segmentation = FALSE;
			else
				segmentation = TRUE;
			}
		else if (*command == 'R')
			set_ranker(command + 1);
		else if (*command == 'f')
			set_focused_ranker(command + 1);
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

