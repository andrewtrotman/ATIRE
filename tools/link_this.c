/*
	LINK_THIS.C
	-----------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"
#include "link_parts.h"

class ANT_link_posting
{
public:
	long docid;
	long times;
public:
	static int compare(const void *a, const void *b);
} ;

class ANT_link_term
{
public:
	char *term;
	long postings_length;			// which is also the number of documents pointed to.
	ANT_link_posting *postings;
	long total_occurences;
public:
	static int compare(const void *a, const void *b);
} ;

class ANT_link
{
public:
	char *place_in_file;
	char *term;
	double gamma;
	long target_document;

public:
	static int compare(const void *a, const void *b);
	static int final_compare(const void *a, const void *b);
} ;

#define MAX_LINKS_IN_FILE (1024 * 1024)
ANT_link all_links_in_file[MAX_LINKS_IN_FILE];
long all_links_in_file_length = 0;

static char buffer[1024 * 1024];


/*
	COUNT_CHAR()
	------------
*/
long count_char(char *buffer, char what)
{
char *ch;
long times;

times = 0;
for (ch = buffer; *ch != '\0'; ch++)
	if (*ch == what)
		times++;

return times;
}

/*
	ANT_LINK::COMPARE()
	-------------------
*/
int ANT_link::compare(const void *a, const void *b)
{
ANT_link *one, *two;
double diff;
long cmp;

one = (ANT_link *)a;
two = (ANT_link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return -1;
else if (diff > 0)
	return 1;
else
	{
	if ((cmp = strcmp(one->term, two->term)) == 0)
		return one->place_in_file - two->place_in_file;
	else
		return cmp;
	}
}

/*
	ANT_LINK::FINAL_COMPARE()
	-------------------------
*/
int ANT_link::final_compare(const void *a, const void *b)
{
ANT_link *one, *two;
double diff;

one = (ANT_link *)a;
two = (ANT_link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return -1;
else if (diff > 0)
	return 1;
else
	return one->place_in_file - two->place_in_file;
}

/*
	ANT_LINK_TERM::COMPARE()
	------------------------
*/
int ANT_link_term::compare(const void *a, const void *b)
{
ANT_link_term *one, *two;

one = (ANT_link_term *)a;
two = (ANT_link_term *)b;

return strcmp(one->term, two->term);
}

/*
	ANT_LINK_POSTING::COMPARE()
	---------------------------
*/
int ANT_link_posting::compare(const void *a, const void *b)
{
ANT_link_posting *one, *two;

one = (ANT_link_posting *)a;
two = (ANT_link_posting *)b;

return two->times - one->times;
}

/*
	READ_INDEX()
	------------
*/
ANT_link_term *read_index(char *filename, long *terms_in_collection)
{
FILE *fp;
ANT_link_term *all_terms, *term;
long unique_terms, postings, current;
char *term_end, *from;

if ((fp = fopen(filename, "rb")) == NULL)
	exit(printf("Cannot index file:%s\n", filename));

fgets(buffer, sizeof(buffer), fp);
sscanf(buffer, "%d", &unique_terms);
term = all_terms = new ANT_link_term [unique_terms];

while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	term_end = strrchr(buffer, ':');
	postings = count_char (term_end, '>');
	term->term = strnnew(buffer, term_end - buffer);
	term->postings_length = postings;
	term->postings = new ANT_link_posting[term->postings_length];
	term->total_occurences = 0;

	from = term_end;
	for (current = 0; current < postings; current++)
		{
		from = strchr(from, '<') + 1;
		sscanf(from, "%d,%d", &(term->postings[current].docid), &(term->postings[current].times));
		term->total_occurences += term->postings[current].times;
		}
	if (term->postings_length > 1)
		qsort(term->postings, term->postings_length, sizeof(term->postings[0]), ANT_link_posting::compare);

	term++;
	}

*terms_in_collection = unique_terms;
return all_terms;
}


/*
	PUSH_LINK()
	-----------
*/
void push_link(char *place_in_file, char *buffer, long docid, double gamma)
{
ANT_link *current;

current = all_links_in_file + all_links_in_file_length;
current->place_in_file = place_in_file;
current->term = _strdup(buffer);
current->gamma = gamma;
current->target_document = docid;

all_links_in_file_length++;
if (all_links_in_file_length >= MAX_LINKS_IN_FILE)
	exit(printf("Too many links in this file (aborting)\n"));
}

/*
	DEDUPLICATE_LINKS()
	-------------------
*/
void deduplicate_links(void)
{
ANT_link *from, *to;

from = to = all_links_in_file + 1;

while (from < all_links_in_file + all_links_in_file_length)
	{
	if (strcmp(from->term, (from - 1)->term) != 0)
		{
		*to = *from;
		from++;
		to++;
		}
	else
		from++;
	}
all_links_in_file_length = to - all_links_in_file;
}

/*
	PRINT_LINKS()
	-------------
*/
void print_links(void)
{
long links_to_print = 250;
long result;

for (result = 0; result < (all_links_in_file_length < links_to_print ? all_links_in_file_length : links_to_print); result++)
	printf("%s %d (gamma:%2.2f)\n", all_links_in_file[result].term, all_links_in_file[result].target_document, all_links_in_file[result].gamma);

puts("<incoming><link><anchor><file>654321.xml</file><offset>445</offset><length>462</length></anchor><linkto><bep>1</bep></linkto></link></incoming>");
puts("</topic>");
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static char *seperators = " ";
ANT_disk disk;
char *file, *token, *where_to;
char **term_list, **first, **last, **current;
ANT_link_term *link_index, key, *index_term, *last_index_term;
long terms_in_index;
double gamma;

if (argc != 3)
	exit(printf("Usage:%s <index> <file_to_link>\n", argv[0]));

puts("Read Index");

link_index = read_index(argv[1], &terms_in_index);

file = disk.read_entire_file(argv[2]);
string_clean(file);
//puts(file);

current = term_list = new char *[strlen(file)];		// this is the worst case by far
for (token = strtok(file, seperators); token != NULL; token = strtok(NULL, seperators))
	*current++ = token;
*current = NULL;

last_index_term = NULL;
for (first = term_list; *first != NULL; first++)
	{
	where_to = buffer;
	for (last = first; *last != NULL; last++)
		{
		if (where_to == buffer)
			{
			strcpy(buffer, *first);
			where_to = buffer + strlen(buffer);
			}
		else
			{
			*where_to++ = ' ';
			strcpy(where_to, *last);
			where_to += strlen(*last);
			}

		key.term = buffer;
		index_term = (ANT_link_term *)bsearch(&key, link_index, terms_in_index, sizeof(*link_index), ANT_link_term::compare);
		if (index_term == NULL)
			{
			if (last_index_term != NULL)
				{
				gamma = (double)last_index_term->postings[0].times / (double)last_index_term->total_occurences;
				push_link(*first, last_index_term->term, last_index_term->postings[0].docid, gamma);
//				printf("%s -> %d (gamma = %d / %d)\n", last_index_term->term, last_index_term->postings[0].docid, last_index_term->postings[0].times, last_index_term->total_occurences);
				last_index_term = NULL;
				break;
				}
			}
		else
			last_index_term = index_term;
		}
	}

qsort(all_links_in_file, all_links_in_file_length, sizeof(*all_links_in_file), ANT_link::compare);

deduplicate_links();
qsort(all_links_in_file, all_links_in_file_length, sizeof(*all_links_in_file), ANT_link::final_compare);
print_links();

delete [] file;
}

