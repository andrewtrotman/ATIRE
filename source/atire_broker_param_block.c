/*
	ATIRE_BROKER_PARAM_BLOCK.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "str.h"
#include "atire_broker_param_block.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ATIRE_BROKER_PARAM_BLOCK::ATIRE_BROKER_PARAM_BLOCK()
	----------------------------------------------------
*/
ATIRE_broker_param_block::ATIRE_broker_param_block(long argc, char *argv[]) : ATIRE_engine_param_block(argc, argv)
{
queries_filename = NULL;
port = 0;
number_of_servers = 0;
servers = NULL;
results_list_length = -1;
}

/*
	ATIRE_BROKER_PARAM_BLOCK::USAGE()
	---------------------------------
*/
void ATIRE_broker_param_block::usage(void)
{
printf("Usage:%s [option...]\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ATIRE_BROKER_PARAM_BLOCK::HELP()
	--------------------------------
*/
void ATIRE_broker_param_block::help(void)
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
puts("-q<filename>    Queries are in file <filename> (format: ANT)");
puts("-q:<port>       Queries from TCP/IP port <port> [default=8088]");
puts("");

puts("SERVERS");
puts("-------");
puts("-s <server:port> ... <server:port>");
puts("");

puts("OUTPUT");
puts("-------");
puts("-l<n>           Length of the results list [default=1500 for batch, default=10 for interactive)]");

exit(0);
}


/*
	ATIRE_BROKER_PARAM_BLOCK::PARSE()
	---------------------------------
*/
long ATIRE_broker_param_block::parse(void)
{
long param, current;
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
		else if (*command == 'l')
			results_list_length = ANT_atoi64(command + 1);
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
		else if (strcmp(command, "people") == 0)
			{
			ANT_credits();
			exit(0);
			}
		else if (*command == 'q')
			{
			if (command[1] == ':')
				port = (unsigned short)(ANT_isdigit(command[2]) ? atol(command + 2) : 8088);
			else
				{
				if (*(command + 1) == '\0' && param < argc - 1) 
					queries_filename = argv[++param];
				else
					queries_filename = command + 1;
				}
			}
		else if (*command == 's')
			{
			current = param + 1;
			while (current < argc && *argv[current] != 0)
				current++;
	
			if ((number_of_servers = current - param - 1) > 0)
				{
				servers = new char *[(size_t)(number_of_servers + 1)];
				for (current = param + 1; current < argc && *argv[current] != 0; current++)
					servers[current - param - 1] = argv[current];
				servers[current - param - 1] = NULL;
				}
			param = current;
			}
		else
			usage();
		}
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
