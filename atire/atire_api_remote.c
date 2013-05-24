/*
	ATIRE_API_REMOTE.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include "ctypes.h"
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
	returns TRUE on success
	         FALSE on fail
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
	returns 0 on network failure
	        1 on success
           -1 on failure
*/
long ATIRE_API_remote::load_index(char *doclist_filename, char *index_filename)
{
long success;
char *result;
std::stringstream buffer;

/*
	Construct the command string and send it off
*/
buffer << "<ATIREloadindex>" << "<doclist>" << doclist_filename << "</doclist><index>" << index_filename << "</index></ATIREloadindex>\n";
if (socket->puts((char *)buffer.str().c_str()) <= 0)
	return 0; /* Broken socket */

if ((result = socket->gets()) == NULL)
	return 0; /* Broken socket */

success = strcmp(result, "<ATIREloadindex>1</ATIREloadindex>\n") == 0 ? 1 : -1;
delete [] result;

return success;
}

/*
	ATIRE_API_REMOTE::DESCRIBE_INDEX()
	----------------------------------
*/
char *ATIRE_API_remote::describe_index()
{
std::stringstream result;
char * got;

if (socket->puts("<ATIREdescribeindex></ATIREdescribeindex>\n") <= 0)
	return NULL;		/* Broken socket */

/*
	Build the result line by line
*/
got = NULL;
do
	{
	delete [] got;

	if ((got = socket->gets()) == NULL)
		return NULL;		/* Broken socket */

	result << got << '\n';
	}
while (strcmp(got, "</ATIREdescribeindex>") != 0 && strcmp(got, "</ATIREerror>") != 0);
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
char *ATIRE_API_remote::search(char *query, long long top_of_page, long long page_length, char *ranker)
{
std::stringstream buffer, result;
char *got;

/*
	Construct the command string and send it off
*/

/*
	Newlines are toxic to atire (parser requires command to be on one line), so strip them out here.
*/
for (got = query; *got != '\0'; got++)
	if (ANT_isspace(*got))
		*got = ' ';

buffer << "<ATIREsearch><query>" << query << "</query>"
		<< "<top>" << top_of_page << "</top><n>" << page_length << "</n>";

if (ranker)
	buffer << "<ranking>" << ranker << "</ranking>";

buffer << "</ATIREsearch>\n";

if (socket->puts((char *)buffer.str().c_str()) <= 0)
	return NULL;

/*
	Build the result line by line
*/
got = NULL;
do
	{
	delete [] got;
	if ((got = socket->gets()) == NULL)
		return NULL;		// socket failure
	result << got << '\n';
	}
while (strcmp(got, "</ATIREsearch>") != 0 && strcmp(got, "</ATIREerror>") != 0);
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
char *ATIRE_API_remote::get_document(long long docid, long long *length)
{
std::stringstream buffer;
char *got, *result;
long size;

*length = 0;
/*
	Send the request
*/
buffer << "<ATIREgetdoc><docid>" << docid << "</docid></ATIREgetdoc>\n";
if (socket->puts((char *)buffer.str().c_str()) <= 0)
	return NULL;

/*
	Get the length of the document
*/
if ((got = socket->gets()) == NULL)
	return NULL;		// socket error
delete [] got;				// <ATIREgetdoc>

size = -1;
do
	{
	if ((got = socket->gets()) == NULL)
		return NULL;		// socket error
	if (strncmp(got, "<length>", 8) == 0)
		size = atol(got + 8);
	delete [] got;
	}
while (size < 0);

result = new char [size];
if (socket->block_read(result, size) == NULL)
	{
	delete [] result;
	result = NULL;
	}
else
	{
	got = socket->gets();
	delete [] got;				// </ATIREgetdoc>
	}

*length = size;
return result;
}

/*
	ATIRE_API_REMOTE::GET_CONNECT_STRING()
	--------------------------------------
*/
char *ATIRE_API_remote::get_connect_string()
{
return connect_string == NULL ? NULL : strnew(connect_string);
}

