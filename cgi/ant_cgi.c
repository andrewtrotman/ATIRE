/*
	ANT_CGI.C
	---------
	set QUERY_STRING=query=the
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "../source/str.h"
#include "../source/channel_socket.h"
#include "../source/maths.h"
#include "../source/disk.h"
#include "header.h"

#define RESULTS_PER_PAGE 10
#define ATIRE 1


/*
	BETWEEN()
	---------
*/
char *between(char *source, char *open_tag, char *close_tag)
{
char *start,*finish;

if ((start = strstr(source, open_tag)) == NULL)
	return NULL;

start += strlen(open_tag);

if ((finish = strstr(start, close_tag)) == NULL)
	return NULL;

return strnnew(start, finish - start);
}

/*
	PROCESS_ATIRE_RESULT()
	--------------------
	<hit><rank>1</rank><id>28220</id><name>WSJ871216-0063</name><rsv>7.00</rsv><title></title></hit>
*/
void process_atire_result(char *result, char *query)
{
long long antid;
long result_number;
char *filename, *title;
double rsv;

result_number = atol(between(result, "<rank>", "</rank>"));
antid = atoll(between(result, "<id>", "</id>"));
filename = between(result, "<name>", "</name>");
rsv = atof(between(result, "<rsv>", "</rsv>"));
title = between(result, "<title>", "</title>");

if (title != NULL && *title != '\0')
	printf("<li><a href=ant_getdoc_cgi.exe?ID=%lld&Q=%s><b>%s</b></a><br>\n%s<br><br>\n\n</li>", antid, query, title, filename);
else
	printf("<li><a href=ant_getdoc_cgi.exe?ID=%lld&Q=%s><b>%s</b></a><br><br>\n\n</li>", antid, query, filename);
}

/*
	PROCESS_RESULT()
	----------------
	1:34:data\142000.xml 420.000000 Symphonic black metal
	<rank>:<ANTID>:<filename> <RSV> <NAME>
*/
void process_result(char *result, char *query)
{
long long antid;
long result_number;
char *filename, *space, *title, *from;
double rsv;

result_number = atol(result);

antid = atoll(from = (strchr(result, ':') + 1));

filename = strchr(from, ':') + 1;

space = filename;
while (isspace(*space))
	space++;
while (!isspace(*space))
	space++;
*space++ = '\0';
while (isspace(*space))
	space++;

rsv = atof(space);

title = strchr(space, ' ');

if (title != NULL && *title != '\0')
	title++;

if (title != NULL && *title != '\0')
	printf("<li><a href=ant_getdoc_cgi.exe?ID=%lld&Q=%s><b>%s</b></a><br>\n%s<br><br>\n\n</li>", antid, query, title, filename);
else
	printf("<li><a href=ant_getdoc_cgi.exe?ID=%lld&Q=%s><b>%s</b></a><br><br>\n\n</li>", antid, query, filename);
}


/*
	MAIN()
	------
*/
int main(void)
{
long hits, current, valid, atire = 0, atire_found = 0, time_taken = 0;
ANT_channel_socket *socket;
char *result, *ch, *query_string = getenv("QUERY_STRING");

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

query_string = strchr(query_string, '=');

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

query_string = strnew(query_string + 1);

/*
	Should really URL decode this string.
*/
valid = 0;
for (ch = query_string; *ch != '\0'; ch++)
	if (!isalnum(*ch))
		*ch = ' ';
	else
		valid++;

/*
	Output stuff
*/
//puts("Pragma: no-cache");
//puts("Cache-Control: no-cache");
puts("Content-Type: text/html\n\n");
if (valid == 0)
	result = "Invalid Query";
else
	{
	socket = new ANT_channel_socket(8088, "localhost");
	socket->puts(query_string);

	if ((result = socket->gets()) == NULL)
		{
		puts("Communications Error");
		return 0;
		}
	if (strncmp(result, "<ATIREsearch>", 13) == 0)
		atire = 1;
	}

hits = 0;

if (atire)
	{
	if ((result = socket->gets()) == NULL)
		{
		puts("Communications Error");
		return 0;
		}
//	<query>the</query><numhits>167095</numhits><time>25</time>
	atire_found = hits = atol(between(result, "<numhits>", "</numhits>"));
	time_taken = atol(between(result, "<time>", "</time>"));
	}
else
	{
	if ((ch = strchr(result, '\'')) != NULL)
		if ((ch = strchr(ch + 1, '\'')) != NULL)
			hits = atol(strpbrk(ch + 1, "1234567890"));
	}

if (hits > RESULTS_PER_PAGE)
	hits = RESULTS_PER_PAGE;

ANT_CGI_header(query_string);
puts("<font size=+1><b>");
if (atire)
	printf("Found %ld documents in %ldms\n", atire_found, time_taken );
else
	puts(result);
puts("</b></font><br>");
puts("<ol>");

for (ch = query_string; *ch != '\0'; ch++)
	if (!isalnum(*ch))
		*ch = '+';

for (current = 0; current < hits; current++)
	{
	if ((result = socket->gets()) == NULL)
		return 0;
	if (atire)
		if (strncmp(result, "<hit>", 5) == 0)
			process_atire_result(result, query_string);
		else
			current--;
	else
		process_result(result, query_string);
	}
puts("</ol>");
puts(ANT_disk::read_entire_file("footer.htm"));

return 0;
}
