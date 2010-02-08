/*
	ANT_GETDOC_CGI.C
	----------------
*/
#include <stdio.h>
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
void process_result(char *result)
{
char *ch, *from = result;

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
char *document;

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

query_string = strchr(query_string, '=');

if (query_string == NULL)
	exit(puts("MISSING_QUERY_STRING:CGI must be called via a web server"));

docid = atoll(query_string + 1);

sprintf(buffer, ".get %lld", docid);

/*
	Output stuff
	We *can* cache the display of a document.
*/
//puts("Pragma: no-cache");
//puts("Cache-Control: no-cache");
puts("Content-Type: text/html\n\n");

socket = new ANT_channel_socket(8088, "localhost");
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

ANT_CGI_header();;
process_result(document);
puts(ANT_disk::read_entire_file("footer.htm"));

return 0;
}
