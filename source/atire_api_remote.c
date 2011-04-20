/*
	ATIRE_API_REMOTE.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include "str.h"
#include "sockets.h"
#include "atire_api_remote.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ATIRE_API_REMOTE::ATIRE_API_REMOTE()
	------------------------------------
*/
ATIRE_API_remote::ATIRE_API_remote(void)
{
connect_string = NULL;
socket = new ANT_socket;
}

/*
	ATIRE_API_REMOTE::~ATIRE_API_REMOTE()
	-------------------------------------
*/
ATIRE_API_remote::~ATIRE_API_remote()
{
delete socket;
delete [] connect_string;
}

/*
	ATIRE_API_REMOTE::OPEN()
	------------------------
*/
long ATIRE_API_remote::open(char *connect_string)
{
long answer;
unsigned short port;
char *pos;

/*
	if we're already connected to a server then disconnect first.
*/
if (this->connect_string != NULL)
	close();

/*
	save the conect string
*/
this->connect_string = strnew(connect_string);

/*
	decode the connect string
*/
if ((pos = strchr(this->connect_string, ':')) == NULL)
	port = 8088;
else
	{
	*pos = '\0';		// change my local copy inplace (remember to put it back later)
	port = (unsigned short)atoi(pos + 1);
	}

/*
	connect
*/
answer = socket->open(this->connect_string, port);

/*
	Fix the connect string
*/
if (pos != NULL)
	*pos = ':';

return answer;
}

/*
	ATIRE_API_REMOTE::CLOSE()
	-------------------------
*/
long ATIRE_API_remote::close(void)
{
socket->close();
delete [] connect_string;
connect_string = NULL;

return TRUE;
}

/*
	ATIRE_API_REMOTE::LOAD_INDEX()
	------------------------------
*/
int ATIRE_API_remote::load_index(char *doclist_filename, char *index_filename)
{
std::stringstream buffer;

/*
	Construct the command string and send it off
*/
buffer << "<ATIREloadindex>" << "<doclist>" << doclist_filename << "</doclist><index>" << index_filename << "</index></ATIREloadindex>\n";
socket->puts((char *)buffer.str().c_str());

char * result = socket->gets();

if (!result)
	return 0; /* Broken socket */

int success = strcmp(result, "<ATIREloadindex>1</ATIREloadindex>\n");

delete [] result;

return success;
}

/*
	ATIRE_API_REMOTE::DESCRIBE_INDEX()
	--------------------------
*/
char *ATIRE_API_remote::describe_index()
{
std::stringstream result;
char * got;

socket->puts("<ATIREdescribeindex></ATIREdescribeindex>\n");

/*
	Build the result line by line
*/
got = NULL;
do
	{
	delete [] got;

	got = socket->gets();

	if (!got) 
		{
		/* Broken socket */
		return NULL;
		}

	result << got << '\n';
	}
while (strcmp(got, "</ATIREdescribeindex>") != 0);
delete [] got;

/*
	Return the result
*/
return strnew(result.str().c_str());
}

/*
	ATIRE_API_REMOTE::SEARCH()
	--------------------------
*/
char *ATIRE_API_remote::search(char *query, long top_of_page, long page_length)
{
std::stringstream buffer, result;
char *clean_query, *current, *got;

/*
	clean the query string by removing all non-alphanumerics
*/
clean_query = strnew(query);
for (current = clean_query; *current != '\0'; current++)
	if (!ANT_isalnum(*current))
		*current = ' ';

/*
	Construct the command string, clean up, and send it off
*/
buffer << "<ATIREsearch><query>" << clean_query << "</query>" << "<top>" << top_of_page << "</top><n>" << page_length << "</n></ATIREsearch>\n";
delete [] clean_query;
socket->puts((char *)buffer.str().c_str());

/*
	Build the result line by line
*/
got = NULL;
do
	{
	delete [] got;
	result << (got = socket->gets()) << '\n';
	}
while (strcmp(got, "</ATIREsearch>") != 0);
delete [] got;

/*
	Return the result
*/
return strnew(result.str().c_str());
}

/*
	ATIRE_API_REMOTE::GET_DOCUMENT()
	--------------------------------
*/
char *ATIRE_API_remote::get_document(long long docid)
{
std::stringstream buffer;
char *got, *result;
long size;

/*
	Send the request
*/
buffer << "<ATIREgetdoc><docid>" << docid << "</docid></ATIREgetdoc>\n";
socket->puts((char *)buffer.str().c_str());

/*
	Get the length of the document
*/
got = socket->gets();
delete [] got;				// <ATIREgetdoc>

size = -1;
do
	{
	got = socket->gets();
	if (strncmp(got, "<length>", 8) == 0)
		size = atol(got + 8);
	delete [] got;
	}
while (size < 0);

result = new char [size];
socket->block_read(result, size);

got = socket->gets();
delete [] got;				// </ATIREgetdoc>

return result;
}

/*
	ATIRE_API_REMOTE::GET_CONNECT_STRING()
	--------------------------------------
*/
char *ATIRE_API_remote::get_connect_string()
{
return strnew(connect_string);
}

