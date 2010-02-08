/*
	ANT_CGI.C
	---------
	set QUERY_STRING=query=the
*/
#include <stdlib.h>
#include <stdio.h>
#include "../source/str.h"
#include "../source/channel_socket.h"
#include "../source/maths.h"
#include "../source/disk.h"
#include "header.h"

/*
	PROCESS_RESULT()
	----------------
	1:34:id:data\142000.xml 420.000000 Symphonic black metal
	<rank>:<ANTID>:<filename> <RSV> <NAME>
*/
void process_result(char *result)
{
long long antid;
long result_number;
char *filename, *space, *title, *from;
double rsv;

result_number = atol(result);
antid = atoll(from = (strchr(result, ':') + 1));

filename = strchr(from, ':') + 1;

space = strchr(result, ' ');
*space++ = '\0';
rsv = atof(space);
title = strchr(space, ' ') + 1;

printf("<tr><td><a href=ant_getdoc_cgi.exe?ID=%lld><b>%s</b></a><br>\n%s<br><br>\n\n</td></tr>", antid, title, filename);
}

/*
	MAIN()
	------
*/
int main(void)
{
long hits, current;
ANT_channel_socket *socket;
char *result, *ch, *query_string = getenv("QUERY_STRING");

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

query_string = strchr(query_string, '=');

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

query_string = strnew(query_string + 1);
/*
	Should really URL decode this string but we'll stick with replacing '+' with ' ' at the moment
*/
for (ch = query_string; *ch != '\0'; ch++)
	if (*ch == '+')
		*ch = ' ';
	else if (*ch == '.')		// outlawed as they are special ANT commands
		*ch = ' ';

/*
	Output stuff
*/
puts("Pragma: no-cache");
puts("Cache-Control: no-cache");
puts("Content-Type: text/html\n\n");

socket = new ANT_channel_socket(8088, "localhost");
socket->puts(query_string);

if ((result = socket->gets()) == NULL)
	{
	puts("Communications Error");
	return 0;
	}

hits = atol(strpbrk(result, "1234567890"));
if (hits > 10)
	hits = 10;

ANT_CGI_header();
//puts("<center><table width=80%>");
for (current = 0; current < hits; current++)
	{
	result = socket->gets();
	process_result(result);
	}
//puts("</center></table>");
puts(ANT_disk::read_entire_file("footer.htm"));

return 0;
}
