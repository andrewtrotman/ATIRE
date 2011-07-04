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
#include "version.h"

const long long MAX_RETRIES = 10;
const char * const PROMPT = "]";		// tribute to Apple
const long MAX_TITLE_LENGTH = 1024;

/*
	PROMPT()
	--------
*/
void prompt(void)
{
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
char *perform_query(ANT_channel *outchannel, ATIRE_API_remote *server, long long topic_id, char *query, long long top_of_page, long long page_length)
{
ATIRE_engine_result_set answers;
char *reply, *TREC;
long long retry;

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
	TREC = answers.serialise_TREC(topic_id, "run", top_of_page, page_length);
	outchannel->puts(TREC);
	
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
long long line, topic_id;
char *command, *query;

puts(ANT_version_string);
/*
	Check the parameters (who are we going to connect to?)
*/
if (argc != 2)
	exit(printf("Usage: %s <server:port>\n", argv[0]));

/*
	read from stdin and write to stdout
*/
inchannel = new ANT_channel_file(NULL);
outchannel = new ANT_channel_file();

/*
	Connect to the server (or broker tree)
*/
if (!open_connection_to_server(&server, argv[1]))
	exit(printf("Cannot connect to:%s\n", argv[1]));

/*
	Now do the searches
*/
line = 0;
prompt();
for (command = inchannel->gets(); command != NULL; prompt(), command = inchannel->gets())
	{
	line++;

	/*
		Parsing to get the topic number
	*/
	strip_space_inplace(command);
	if (strcmp(command, ".quit") == 0)
		{
		delete [] command;
		break;
		}
	else if (*command == '\0')
		delete [] command;			// ignore blank lines
	else
		{
		topic_id = -1;				// check for a TREC (or INEX) topic ID
		if (!ANT_isdigit(*command))
			query = command;
		else
			{
			topic_id = atol(command);
			if ((query = strchr(command, ' ')) == NULL)
				{
				printf("Line %lld: Skipping badly formed query:'%s'\n", line, command);
				continue;
				}
			}
		perform_query(outchannel, &server, topic_id, query, 1, 10);
		delete [] command;
		}
	}

return 0;
}

