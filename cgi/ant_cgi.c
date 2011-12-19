/*
	ANT_CGI.C
	---------
	set QUERY_STRING=query=the
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "../source/str.h"
#include "../source/atire_api_remote.h"
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
	<hit><rank>1</rank><id>28220</id><name>WSJ871216-0063</name><rsv>7.00</rsv><title></title><snippet></snippet></hit>
*/
void process_atire_result(char *result, char *query)
{
long long antid;
long result_number;
char *filename, *title, *snippet;
double rsv;

result_number = atol(between(result, "<rank>", "</rank>"));
antid = atoll(between(result, "<id>", "</id>"));
filename = between(result, "<name>", "</name>");
rsv = atof(between(result, "<rsv>", "</rsv>"));
title = between(result, "<title>", "</title>");
snippet = between(result, "<snippet>", "</snippet>");

printf("<li><a href=ant_getdoc_cgi.exe?ID=%lld&Q=%s><b>%s</b></a><br>", antid, query, filename);

if (title != NULL && *title != '\0')
	printf("%s<br>", title);
if (snippet != NULL && *snippet != '\0')
	printf("<font color=#F00000><b>%s</b></font><br>\n", snippet);

printf("</li><br>");
}

/*
	MAIN()
	------
*/
int main(void)
{
long hits, current, valid, atire_found = 0, time_taken = 0;
ATIRE_API_remote socket;
char *start, *result, *ch, *query_string = getenv("QUERY_STRING");

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
	socket.open("localhost:8088");
	result = socket.search(query_string, 1,10);

	if (result == NULL || strstr(result, "<ATIREerror>") != NULL)
		{
		puts("Communications Error");
		return 0;
		}
	}

hits = 0;

//	<query>the</query><numhits>167095</numhits><time>25</time>
atire_found = hits = atol(between(result, "<numhits>", "</numhits>"));
time_taken = atol(between(result, "<time>", "</time>"));

if (hits > RESULTS_PER_PAGE)
	hits = RESULTS_PER_PAGE;

ANT_CGI_header(query_string);
puts("<font size=+1><b>");
printf("Found %ld documents in %ldms\n", atire_found, time_taken );
puts("</b></font><br>");
puts("<ol>");

for (ch = query_string; *ch != '\0'; ch++)
	if (!isalnum(*ch))
		*ch = '+';


start = result;
for (current = 0; current < hits; current++)
	{
	start = strstr(start, "<hit>");
	process_atire_result(start, query_string);
	start++;
	}
puts("</ol>");
puts(ANT_disk::read_entire_file("footer.htm"));

return 0;
}
