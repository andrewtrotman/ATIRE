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
retries = 0;
documents = 0;
this->connect_string = strnew(connect_string);
server = new ATIRE_API_remote;
open_connection_to_server();
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
	ATIRE_BROKE_ENGINE::OPEN_CONNECTION_TO_SERVER()
	-----------------------------------------------
*/
long ATIRE_broke_engine::open_connection_to_server(long voice)
{
char *got, *docnum;

if (!server->open(this->connect_string))
	{
	if (voice != QUIET)
		printf("Cannot open connection to %s\n", connect_string);
	return false;
	}

if ((got = server->describe_index()) == NULL)
	return false;

if ((docnum = strstr(got, "<docnum>")) == NULL)
	documents = 0;
else
	documents = ANT_atoi64(docnum + 8);
delete [] got;

return true;
}

/*
	ATIRE_BROKE_ENGINE::RETRY()
	---------------------------
*/
long ATIRE_broke_engine::retry(void)
{
long got;

if (++retries > MAX_RETRIES)
	{
	printf("Maximum number of retries (%ld) exceeded, knocking out %s for reinclusion at a later date\n", MAX_RETRIES, connect_string);
	return -1;
	}
printf("The connection to %s has been lost... reconnecting... ", connect_string);
if ((got = open_connection_to_server(QUIET)) != 0)
	printf("success\n");
else
	printf("failed to reconnect\n");

return got ? 1 : 0;
}

/*
	ATIRE_BROKE_ENGINE::SEARCH()
	----------------------------
*/
char *ATIRE_broke_engine::search(char *query, long long top_of_page, long long page_length)
{
char *got;

retries = 0;
do
	if ((got = server->search(query, top_of_page, page_length)) == NULL)
		if (retry() == -1)
			return NULL;
while (got == NULL);

return got;
}

/*
	ATIRE_BROKE_ENGINE::DESCRIBE_INDEX()
	------------------------------------
*/
char *ATIRE_broke_engine::describe_index(void)
{
char *got;

retries = 0;
do
	if ((got = server->describe_index()) == NULL)
		if (retry() == -1)
			return NULL;
while (got == NULL);

return got;
}

/*
	ATIRE_BROKE_ENGINE::GET_DOCUMENT()
	----------------------------------
*/
char *ATIRE_broke_engine::get_document(char *document_buffer, long long *current_document_length, long long id)
{
char *document;

*document_buffer = '\0';
document = server->get_document(id, current_document_length);
memcpy(document_buffer, document, (size_t)*current_document_length);
delete [] document;

return *document_buffer == '\0' ? NULL : document_buffer;
}
