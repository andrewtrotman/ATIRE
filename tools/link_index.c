/*
	LINK_INDEX.C
	------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"
#include "link_parts.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

char buffer[1024 * 1024];

class ANT_link_element
{
public:
	char *term;
	long docid;
	long anchor_docid;
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

if ((cmp = strcmp(one->term, two->term)) == 0)				// first by term
	if ((cmp = one->docid - two->docid) == 0)				// then by destination document ID
		cmp = one->anchor_docid - two->anchor_docid;		// then by source document ID

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
long lines, current, last_docid, times, unique_terms, last_anchor_docid, anchor_times;
char *file, *ch, *last_string;
long lowercase_only;

if (argc != 2 && argc != 3)
	exit(printf("Usage:%s <infile> [-lowercase]\n", argv[0]));

lowercase_only = FALSE;
if (argc == 3)
	{
	if (strcmp(argv[2], "-lowercase") == 0)
		lowercase_only = TRUE;
	else
		exit(printf("unknown parameter:%s\n", argv[2]));
	}

if ((file = disk.read_entire_file(argv[1])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[1]));

lines = 0;
for (ch = file; *ch != '\0'; ch++)
	if (*ch == '\n')
		lines++;
	else if (*ch == '\r')
		*ch = ' ';		// convert '\r' into ' '

link_list = new ANT_link_element[lines];

lines = 0;
ch  = file;
while (*ch != '\0')
	{
	link_list[lines].anchor_docid = atol(ch);
	ch = strchr(ch, ':') + 1;
	link_list[lines].docid = atol(ch);
	link_list[lines].term = strchr(ch, ':') + 1;
	if ((ch = strchr(ch, '\n')) == NULL)
		break;
	*ch = '\0';			// NULL terminate the string
	string_clean(link_list[lines].term, lowercase_only);

	lines++;
	ch++;
	}
qsort(link_list, lines, sizeof(*link_list), ANT_link_element::compare);

last_string = "\n";
unique_terms = 0;
for (current = 0; current < lines; current++)
	{
	if (strcmp(link_list[current].term, last_string) != 0)
		unique_terms++;
	last_string = link_list[current].term;
	}
printf("%d terms\n", unique_terms);

last_string = "Z";
last_docid = -1;
last_anchor_docid = -1;
times = 0;
anchor_times = 0;
for (current = 0; current < lines; current++)
	{
	if (strcmp(link_list[current].term, last_string) != 0)
		{
		if (current != 0)
			printf("<%d,%d,%d>\n", last_docid, anchor_times, times);
		printf("%s:", link_list[current].term);
		last_docid = link_list[current].docid;
		last_anchor_docid = link_list[current].anchor_docid;
		times = 1;
		anchor_times = 1;
		}
	else
		if (last_docid == link_list[current].docid)
			{
			times++;
			if (last_anchor_docid != link_list[current].anchor_docid)
				anchor_times++;				// different source documents
			}
		else
			{
			printf("<%d,%d,%d>", last_docid, anchor_times, times);
			times = 1;
			anchor_times = 1;
			}

	last_string = link_list[current].term;
	last_docid = link_list[current].docid;
	last_anchor_docid = link_list[current].anchor_docid;
	}
printf("<%d,%d,%d>", last_docid, anchor_times, times);

fprintf(stderr, "%s Completed\n", argv[0]);
return 0;
}
