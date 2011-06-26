/*
	ATIRE_BROKE_ENGINE.C
	--------------------
*/
#include <stdio.h>
#include "str.h"
#include "atire_broke_engine.h"
#include "atire_api_remote.h"

/*
	ATIRE_BROKE_ENGINE::ATIRE_BROKE_ENGINE()
	----------------------------------------
*/
ATIRE_broke_engine::ATIRE_broke_engine(char *connect_string)
{
this->connect_string = strnew(connect_string);
server = new ATIRE_API_remote;
open_connection_to_server();
}

/*
	ATIRE_BROKE_ENGINE::OPEN_CONNECTION_TO_SERVER()
	-----------------------------------------------
*/
long ATIRE_broke_engine::open_connection_to_server(long voice)
{
if (!server->open(this->connect_string))
	{
	if (voice != QUIET)
		printf("Cannot open connection to %s\n", connect_string);
	return false;
	}

return true;
}

/*
	ATIRE_BROKE_ENGINE::~ATIRE_BROKE_ENGINE()
	-----------------------------------------
*/
ATIRE_broke_engine::~ATIRE_broke_engine()
{
server->close();
delete server;
}

/*
	ATIRE_BROKE_ENGINE::SEARCH()
	----------------------------
*/
char *ATIRE_broke_engine::search(char *query, long long top_of_page, long long page_length)
{
long retries;
char *got;

retries = 0;
do
	{
	if ((got = server->search(query, top_of_page, page_length)) == NULL)
		{
		if (++retries > MAX_RETRIES)
			{
			printf("Maximum number of retries (%ld) exceeded, knocking out %s for reinclusion at a later date\n", MAX_RETRIES, connect_string);
			return NULL;
			}
		printf("The connection to %s has been lost... reconnecting... ", connect_string);
		if (open_connection_to_server(QUIET))
			printf("success\n");
		else
			printf("failed to reconnect\n");
		}
	}
while (got == NULL);

return got;
}
