/*
	ATIRE_CLIENT_PARAM_BLOCK.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
	#define unlink _unlink
#else
	#include <unistd.h>
#endif

#include "version.h"
#include "str.h"
#include "atire_client_param_block.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

static char *default_output_filename = "ant.out";

/*
	ATIRE_CLIENT_PARAM_BLOCK()
	--------------------------
*/
ATIRE_client_param_block::ATIRE_client_param_block(long argc, char *argv[]) : ATIRE_engine_param_block(argc, argv)
{
connect_string = "localhost:8088";
queries_filename = NULL;
queries_format = QUERIES_ANT;
queries_fields = "t";
results_list_length = -1;

output_forum = NONE;
output_filename = default_output_filename;
run_name = "Unknown";
group_name = "Unknown";
run_description = "Unknown";
}

/*
	ATIRE_CLIENT_PARAM_BLOCK::USAGE()
	---------------------------------
*/
void ATIRE_client_param_block::usage(void)
{
printf("Usage:%s [option...]\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ATIRE_CLIENT_PARAM_BLOCK::HELP()
	--------------------------------
*/
void ATIRE_client_param_block::help(void)
{
puts(ANT_version_string);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H        Display this help message");
puts("-nologo         Suppress banner");
puts("-people         Display credits");
puts("");

puts("INPUT");
puts("-----");
puts("-q<filename>    Queries are in file <filename> [default format: ANT]");
puts("-QN:<t><n><d>   NIST (TREC) query file (from trec.nist.gov) t=title, n=narrative, d=description [default=t]");
puts("");

puts("SERVERS");
puts("-------");
puts("-s<server:port> Connect to ATIRE on given port on given server");
puts("");

puts("OUTPUT");
puts("-------");
puts("-l<n>           Length of the results list [default=1500 for batch, default=10 for interactive)]");
puts("-o<filename>    Output filename for the run (destructive) [default=ant.out]");
puts("");

puts("TREC SPECIFIC");
puts("-------------");
puts("-e[-ts]         Export a run file for use in an Evaluation Forum");
puts("   -            Don't generate a run file [default]");
puts("   t            TREC run format");
puts("   s            INEX snippet retrieval 2011");
puts("-E[forum]       Use defaults for the given forum");
puts("  tw11<name>    TREC web track 2011 (-et -l10000 -n<name> -o<name>");
puts("  is11<name>    INEX snippet track 2011 (-es -l500 -n<name> -o<name> -i<name> -d<name>");
puts("-n<name>        Run is named <name> [default=unknown]");
puts("-i<id>          Forum participant id is <id> [default=unknown]");
puts("-d<description> Run description is <description> [default=unknown]");
exit(0);
}

/*
	ATIRE_CLIENT_PARAM_BLOCK::EXPORT_FORMAT()
	-----------------------------------------
*/
void ATIRE_client_param_block::export_format(char *forum)
{
do
	{
	switch (*forum)
		{
		case '-' : output_forum = NONE;   break;
		case 't' : output_forum = TREC;   break;
		case 's' : output_forum = INEX_SNIPPET;   break;
		default  : exit(printf("Unknown export format: '%c'\n", *forum)); break;
		}
	forum++;
	}
while (*forum != '\0');
}

/*
	ATIRE_CLIENT_PARAM_BLOCK::EXPORT_FORMAT_DEFAULTS()
	--------------------------------------------------
*/
void ATIRE_client_param_block::export_format_defaults(char *forum)
{
if (strncmp(forum, "tw11", 4) == 0)
	{
	results_list_length = 10000;
	output_forum = TREC;
	output_filename = run_name = forum + 4;

	strip_space_inplace(output_filename);
	if (*output_filename == '\0')
		output_filename = run_name = default_output_filename;

	unlink(output_filename);
	}
else if (strncmp(forum, "is11", 4) == 0)
	{
	results_list_length = 500;
	output_forum = INEX_SNIPPET;
	group_name = run_description = output_filename = run_name = forum + 4;

	strip_space_inplace(output_filename);
	if (*output_filename == '\0')
		group_name = run_description = output_filename = run_name = default_output_filename;

	unlink(output_filename);
	}
else
	exit(printf("Unknown forum: '%s'\n", forum));
}

/*
	ATIRE_CLIENT_PARAM_BLOCK::QUERY_FORMAT()
	----------------------------------------
*/
void ATIRE_client_param_block::query_format(char *command)
{
char *fields, *check;

fields = strchr(command, ':');
if (fields == NULL)
	queries_fields = "t";
else
	{
	fields++;
	if (*fields == '\0')
		queries_fields = "t";
	else
		{
		for (check = fields; *check != '\0'; check++)
			if (strchr("tdn", *check) == NULL)
				exit(printf("Unknown field combination to extract from TREC file:%s\n", fields));
		queries_fields = fields;
		}
	}
queries_format = QUERIES_TREC;
}

/*
	ATIRE_CLIENT_PARAM_BLOCK::PARSE()
	---------------------------------
*/
long ATIRE_client_param_block::parse(void)
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
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
		else if (strcmp(command, "people") == 0)
			{
			ANT_credits();
			exit(0);
			}
		else if (*command == 'd')
			run_description = command + 1;
		else if (*command == 'e')
			export_format(command + 1);
		else if (*command == 'E')
			export_format_defaults(command + 1);
		else if (*command == 'i')
			group_name = command + 1;
		else if (*command == 'l')
			results_list_length = ANT_atoi64(command + 1);
		else if (*command == 'n')
			run_name = command + 1;
		else if (*command == 'o')
			{
			if (*(command + 1) != '\0')
				{
				output_filename = command + 1;
				unlink(output_filename);
				}
			}
		else if (*command == 'q')
			{
			if (*(command + 1) == '\0' && param < argc - 1) 
				queries_filename = argv[++param];
			else
				queries_filename = command + 1;
			}
		else if (*command == 'Q')
			query_format(command);
		else if (*command == 's')
			{
			if (*(command + 1) == '\0' && param < argc - 1) 
				connect_string = argv[++param];
			else
				connect_string = command + 1;
			}
		}	
	else
		usage();
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
