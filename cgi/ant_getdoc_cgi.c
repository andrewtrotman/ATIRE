/*
	ANT_GETDOC_CGI.C
	----------------
*/
#include <stdio.h>
#include <ctype.h>
#include "../source/maths.h"
#include "../source/str.h"
#include "../source/channel_socket.h"
#include "../source/disk.h"
#include "header.h"

/*
	PROCESS_RESULT()
	----------------
	XML file
*/
void process_result(unsigned char *result)
{
unsigned char *ch, *from = result;
long enter_highlight = 1;

for (ch = result; *ch != '\0'; ch++)
	{
	if (*ch == '<')
		{
		fwrite(from, ch - from, 1, stdout);
		fwrite("&lt;", 4, 1, stdout);
		from = ch + 1;
		}
	else if (*ch == '>')
		{
		fwrite(from, ch - from, 1, stdout);
		fwrite("&gt;", 4, 1, stdout);
		from = ch + 1;
		}
	else if (*ch == '\n')
		{
		fwrite(from, ch - from, 1, stdout);
		fwrite("<br>", 4, 1, stdout);
		from = ch + 1;
		}
	else if (*ch == 0xFF)
		{
		fwrite(from, ch - from, 1, stdout);
		if (enter_highlight)
			fwrite("<font color=F00000><b>", 22, 1, stdout);
		else
			fwrite("</b></font>",11, 1, stdout);
		enter_highlight = !enter_highlight;
		from = ch + 1;
		}
	}
}

/*
	MAIN()
	------
*/
int main(void)
{
char buffer[1024];
ANT_channel_socket *socket;
char *result, *query_string = getenv("QUERY_STRING");
long long docid, size;
char *document, *hightlight_terms, *ch;

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

query_string = strchr(query_string, '=');

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

docid = atoll(query_string + 1);
if ((hightlight_terms = strchr(query_string + 1, '=')) == NULL)
	hightlight_terms = (char *)"";

sprintf(buffer, ".get %lld %s", docid, hightlight_terms);
for (ch = buffer + 1; *ch != '\0'; ch++)
	if (!isalnum(*ch))
		*ch = ' ';

/*
	Output stuff
	We *can* cache the display of a document.
*/
//puts("Pragma: no-cache");
//puts("Cache-Control: no-cache");
puts("Content-Type: text/html;charset=UTF-8\n\n");

socket = new ANT_channel_socket(8088, (char *)"localhost");
socket->puts(buffer);

if ((result = socket->gets()) == NULL)
	{
	puts("Communications Error");
	return 0;
	}
size = atoll(result);
document = new char[(size_t)(size + 1)];

socket->read(document, size);
document[size] = '\0';

ANT_CGI_header();
process_result((unsigned char *)document);
puts(ANT_disk::read_entire_file((char *)"footer.htm"));

return 0;
}
