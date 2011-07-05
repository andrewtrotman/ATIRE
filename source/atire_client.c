/*
	ATIRE_CLIENT.C
	--------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "ctypes.h"
#include "atire_api_remote.h"
#include "channel_file.h"
#include "channel_socket.h"
#include "atire_engine_result_set.h"
#include "atire_client_param_block.h"
#include "version.h"

const long long MAX_RETRIES = 10;
const char * const PROMPT = "]";		// tribute to Apple
const long MAX_TITLE_LENGTH = 1024;

/*
	PROMPT()
	--------
*/
void prompt(ATIRE_client_param_block *params)
{
if (params->output_forum == ATIRE_client_param_block::NONE)
	printf(PROMPT);
}

/*
	OPEN_CONNECTION_TO_SERVER()
	---------------------------
*/
long open_connection_to_server(ATIRE_API_remote *server, char *connect_string)
{
if (!server->open(connect_string))
	{
	printf("Cannot open connection to %s\n", connect_string);
	return false;
	}

return true;
}

/*
	PERFORM_QUERY()
	---------------
*/
char *perform_query(ANT_channel *outchannel, ATIRE_client_param_block *params, ATIRE_API_remote *server, long long topic_id, char *query, long long top_of_page, long long page_length)
{
ATIRE_engine_result_set answers;
char *reply, *TREC;
long long retry;

//printf("Topic:%lld query:%s\n", topic_id, query);

retry = 0;
do
	{
	if ((reply = server->search(query, top_of_page, page_length)) == NULL)
		retry++;
	else
		break;

	printf("Connection lost... retrying\n");
	open_connection_to_server(server, server->get_connect_string());
	}
while (retry < MAX_RETRIES);

if (retry >= MAX_RETRIES)
	printf("Timed out\n");
else if (retry > 0)
	printf("Reconnected\n");

if (reply != NULL)
	{
	answers.add(reply);
	TREC = answers.serialise_TREC(topic_id, params->run_name, top_of_page, page_length);
	outchannel->write(TREC);
	
	delete [] reply;
	delete [] TREC;
	}

return reply;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_channel *inchannel, *outchannel;
ATIRE_API_remote server;
ATIRE_client_param_block params(argc, argv);
long long line, topic_id;
char *command, *query;

/*
	Parse the parameters
*/
params.parse();

/*
	Print the welcome message
*/
if (params.logo)
	puts(ANT_version_string);

/*
	read from stdin and write to stdout
*/
inchannel = new ANT_channel_file(params.queries_filename);
outchannel = new ANT_channel_file(params.output_forum == ATIRE_client_param_block::NONE ? NULL : params.output_filename);

/*
	Connect to the server (or broker tree)
*/
if (!open_connection_to_server(&server, params.connect_string))
	exit(printf("Cannot connect to:%s\n", params.connect_string));

/*
	Now do the searches
*/
line = 0;
prompt(&params);
for (command = inchannel->gets(); command != NULL; prompt(&params), command = inchannel->gets())
	{
	/*
		inc line number and remove spaces from the beginning and end of the input line
	*/
	line++;
	strip_space_inplace(command);

	/*
		commands start with a dot (.)
	*/
	if (strcmp(command, ".quit") == 0)
		{
		delete [] command;			// quit the program
		break;
		}
	else if (*command == '\0')
		delete [] command;			// ignore blank lines
	else
		{
		/*
			we're a query
		*/
		topic_id = -1;
		query = NULL;

		/*
			does the query start with a topic number?
		*/
		if (params.output_forum == ATIRE_client_param_block::NONE)
			query = command;
		else
			{
			/*
				seperate the topic id from the query
			*/
			if (ANT_isdigit(*command))
				{
				topic_id = atol(command);
				query = strpbrk(command, " :");
				}
			/*
				Do we have an error? (no topic id or no query)
			*/
			if (topic_id < 0 || query == NULL)
				{
				printf("Line %lld: Skipping badly formed query:'%s'\n", line, command);
				continue;
				}
			}
		/*
			Now do the query and then clean up
		*/
		perform_query(outchannel, &params, &server, topic_id, query, 1, params.results_list_length);
		delete [] command;
		}
	}

/*
	finished
*/
delete inchannel;
delete outchannel;

return 0;
}

