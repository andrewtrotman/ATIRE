/*
	LINK_EXTRACT_PASS2.C
	--------------------
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

static char buffer[1024 * 1024];

/*
	class ANT_LINK_EXTRACT_TERM
	---------------------------
*/
class ANT_link_extract_term
{
public:
	char *term;
	long last_docid;
	long docs_containing_term;
	long total_occurences;
public:
	static int compare(const void *a, const void *b);
} ;

/*
	ANT_LINK_EXTRACT_TERM::COMPARE()
	--------------------------------
*/
int ANT_link_extract_term::compare(const void *a, const void *b)
{
ANT_link_extract_term *one, *two;

one = (ANT_link_extract_term *)a;
two = (ANT_link_extract_term *)b;

return strcmp(one->term, two->term);
}

/*
	READ_INDEX()
	------------
*/
ANT_link_extract_term *read_index(char *filename, long *terms_in_collection)
{
FILE *fp;
ANT_link_extract_term *all_terms, *term;
long unique_terms;
char *term_end;

if ((fp = fopen(filename, "rb")) == NULL)
	exit(printf("Cannot index file:%s\n", filename));

fgets(buffer, sizeof(buffer), fp);
sscanf(buffer, "%d", &unique_terms);
term = all_terms = new ANT_link_extract_term [unique_terms];

while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	term_end = strrchr(buffer, ':');
	term->term = strnnew(buffer, term_end - buffer);
	term->docs_containing_term = 0;
	term->total_occurences = 0;
	term->last_docid = -1;
	term++;
	}

*terms_in_collection = unique_terms;
return all_terms;
}

/*
	PRINT_ANSWER()
	--------------
*/
void print_answer(ANT_link_extract_term *index, long terms_in_index)
{
long current;

for (current = 0; current < terms_in_index; current++)
	printf("%d:%d:%s\n", index[current].docs_containing_term, index[current].total_occurences, index[current].term);
}

/*
	FIND_TERM_IN_LIST()
	-------------------
*/
ANT_link_extract_term *find_term_in_list(char *value, ANT_link_extract_term *list, long list_length)
{
long low, high, mid;

low = 0;
high = list_length;
while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp(list[mid].term, value) < 0)
		low = mid + 1;
	else
		high = mid;
	}

if ((low < list_length) && (strcmp(value, list[low].term) == 0))
	return &list[low];		// match
else
	{
	if (low < list_length)
		return &list[low];		// not found in list but not after the last term in the list
	else
		return NULL;
	}
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static char *seperators = " ";
ANT_disk disk;
char *file, *token, *where_to, *filename;
char **term_list, **first, **last, **current;
ANT_link_extract_term *link_index, *index_term;
long terms_in_index, current_docid, param, file_number;
long lowercase_only, first_param;

if (argc < 3)
	exit(printf("Usage:%s [-lowercase] <index> <file_to_link> ...\n", argv[0]));

first_param = 1;
lowercase_only = FALSE;
if (*argv[1] == '-')
	{
	if (strcmp(argv[1], "-lowercase") == 0)
		{
		lowercase_only = TRUE;
		first_param = 2;
		}
	else
		exit(printf("Unknown parameter:%s\n", argv[1]));
	}


link_index = read_index(argv[first_param], &terms_in_index);

file_number = 1;
for (param = first_param + 1; param < argc; param++)
	{
	filename = disk.get_first_filename(argv[param]);
	file = disk.read_entire_file(filename);
	while (file != NULL)
		{
		current_docid = get_doc_id(file);
//		printf("ID:%d\n", current_docid);
		string_clean(file, lowercase_only);

		current = term_list = new char *[strlen(file)];		// this is the worst case by far
		for (token = strtok(file, seperators); token != NULL; token = strtok(NULL, seperators))
			*current++ = token;
		*current = NULL;

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

				index_term = find_term_in_list(buffer, link_index, terms_in_index);

				if (index_term == NULL)
					break;		// we're after the last term in the list so can stop because we can't be a substring

				if (strcmp(buffer, index_term->term) == 0)		// we're a term in the list
					{
					index_term->total_occurences++;
					if (index_term->last_docid != current_docid)
						{
						index_term->last_docid = current_docid;
						index_term->docs_containing_term++;
						}
					}
				else
					{
					if (strncmp(buffer, index_term->term, strlen(buffer)) != 0)
						break;		// we're a not a substring so we can't find a longer term
					}
				}
			}
		delete [] term_list;
		delete [] file;

		if (file_number % 1000 == 0)
			fprintf(stderr, "Files processed:%d\n", file_number);
		file_number++;

		filename = disk.get_next_filename();
		file = disk.read_entire_file(filename);
		}
	}

print_answer(link_index, terms_in_index);

fprintf(stderr, "%s Completed\n", argv[0]);

return 0;
}

