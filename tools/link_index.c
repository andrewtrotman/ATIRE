/*
	LINK_INDEX.C
	------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"

char buffer[1024 * 1024];

class ANT_link_element
{
public:
	char *term;
	long docid;
public:
	static int compare(const void *a, const void *b);
} ;

/*
	ANT_LINK_ELEMENT::COMPARE()
	---------------------------
*/
int ANT_link_element::compare(const void *a, const void *b)
{
ANT_link_element *one, *two;
int cmp;

one = (ANT_link_element *)a;
two = (ANT_link_element *)b;

if ((cmp = strcmp(one->term, two->term)) == 0)
	cmp = one->docid - two->docid;

return cmp;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_link_element *link_list;
ANT_disk disk;
long lines, current, last_docid, times;
char *file, *ch, *into, *tmp, *last_string;

if (argc != 2)
	exit(printf("Usage:%s <infile>\n", argv[0]));

puts("Loading File");
if ((file = disk.read_entire_file(argv[1])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[1]));

puts("Count number of links");
lines = 0;
for (ch = file; *ch != '\0'; ch++)
	if (*ch == '\n')
		lines++;
	else if (*ch == '\r')
		*ch = ' ';		// convert '\r' into ' '

printf("File:%s has %d links and is %d in length\n", argv[1], lines, ch - file);
link_list = new ANT_link_element[lines];

puts("Loading links");
lines = 0;
ch  = file;
while (*ch != '\0')
	{
	if ((link_list[lines].docid = atol(ch)) == 0)
		{
		into = buffer;
		while (*ch != '\n' && *ch != '\r')
			*into++ = *ch++;
		*into = '\0';
		printf("Warning Line %d:Cannot extract DOC_ID (text:%s)\n", lines, buffer);
		}
	link_list[lines].term = strchr(ch, ':') + 1;
	lines++;
	if ((ch = strchr(ch, '\n')) == NULL)
		break;
	tmp = ch;
	while (isspace(*ch))
		{
		*ch = '\0';
		*ch--;
		}
	ch = tmp;
//	*ch = '\0';			// NULL terminate the string
	ch++;
	}
printf("Sort %d links\n", lines);
qsort(link_list, lines, sizeof(*link_list), ANT_link_element::compare);

puts("Write out the answers");
last_string = "Z";
last_docid = -1;
times = 0;
for (current = 0; current < lines; current++)
	{
	if (strcmp(link_list[current].term, last_string) != 0)
		{
		if (current != 0)
			printf("<%d,%d>\n", last_docid, times);
		printf("%s:", link_list[current].term);
		last_docid = link_list[current].docid;
		times = 1;
		}
	else
		if (last_docid == link_list[current].docid)
			times++;
		else
			{
			printf("<%d,%d>", last_docid, times);
			times = 1;
			}

	last_string = link_list[current].term;
	last_docid = link_list[current].docid;
	}
printf("<%d,%d>", last_docid, times);
}

