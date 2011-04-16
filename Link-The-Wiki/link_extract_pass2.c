/*
	LINK_EXTRACT_PASS2.C
	--------------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"
#include "../source/directory_iterator_recursive.h"
#include "../source/parser.h"
#include "link_parts.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

//static char buffer[1024 * 1024];
static long chinese;

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
char buffer[1024 * 1024];

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
	{
	printf("%d:%d:%s\n", index[current].docs_containing_term, index[current].total_occurences, index[current].term);
	}
}

/*
	CREATE_UTF8_TOKEN_LIST()
	------------------------
*/
int create_utf8_token_list(char *s, char **term_list)
{
char *start, *token, *where_to = s;
long token_len = 0, term_count;
char **current = term_list;

term_count = 0;
while (*where_to != '\0')
	{
	while (isspace(*where_to))
		++where_to;

	start = where_to;
	if ((*where_to & 0x80) &&ANT_parser::isutf8(where_to))
		{
		token_len = ANT_parser::utf8_bytes(where_to);
		where_to += token_len;
		}
	else
		while (*where_to != '\0' && !isspace(*where_to) &&  !((*where_to & 0x80) && ANT_parser::isutf8(where_to)))
			{
			++token_len;
			++where_to;
			}

	*current = token = new char[token_len + 1];
	strncpy(*current, start, token_len);
	token[token_len] = '\0';
	++current;
	token_len = 0;
	++term_count;
	}

*current = NULL;
return term_count;
}

/*
	FREE_UTF8_TOKEN_LIST()
	----------------------
*/
void free_utf8_token_list(char **term_list)
{
char **current = term_list;
while (*current != NULL)
	{
	delete [] *current;
	++current;
	}
}

/*
	STRING_COMPARE()
	----------------
*/
int string_compare(char *s1, char *s2)
{
int min_len, cmp;
if (chinese && (strchr(s1, ' ') != NULL || strchr(s2, ' ') != NULL)) // we don't need token comparison for all, only those has space in it
	cmp = utf8_token_compare(s1, s2);
else
	{
	min_len = strlen(s1) > strlen(s2) ? strlen(s2) : strlen(s1);
	cmp = memcmp(s1, s2, min_len);
	if (cmp == 0 && strlen(s1) != strlen(s2))
		cmp = strlen(s1) < strlen(s2) ? -1 : 1;
	}
return cmp;
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
	if (string_compare(list[mid].term, value) < 0)
		low = mid + 1;
	else
		high = mid;
	}

if ((low < list_length) && (string_compare(value, list[low].term) == 0))
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
char *file, *token, *where_to, *filename;			// *start;
char **term_list, **first, **last, **current;
ANT_link_extract_term *link_index, *index_term;
long terms_in_index, current_docid, param, file_number;
long lowercase_only, first_param;
long is_utf8_token, cmp, is_substring = FALSE;				// token_len
char *command;
ANT_directory_iterator_object file_object;

char buffer[1024 * 1024];

if (argc < 3)
	exit(printf("Usage:%s [-chinese] [-lowercase] <index> <file_to_link> ...\n", argv[0]));

first_param = 1;
lowercase_only = FALSE;
chinese = FALSE;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-')
		{
		command = argv[param] + 1;
		if (strcmp(command, "lowercase") == 0)
			{
			lowercase_only = TRUE;
			++first_param;
			}
		else if (strcmp(command, "chinese") == 0)
			{
			chinese = TRUE;
			++first_param;
			}
		else
			exit(printf("Unknown parameter:%s\n", argv[param]));
		}
	}

link_index = read_index(argv[first_param], &terms_in_index);

file_number = 1;
for (param = first_param + 1; param < argc; param++)
	{
	ANT_directory_iterator_recursive disk(argv[param]);  // make the recursive pattern matching as for default files reading
	if (disk.first(&file_object) == NULL)
		file = filename = NULL;
	else
		{
		filename = file_object.filename;
		file = ANT_disk::read_entire_file(filename);
		}
	while (file != NULL)
		{
		current_docid = get_doc_id(file);
//		printf("ID:%d\n", current_docid);
		string_clean(file, lowercase_only, TRUE);

		current = term_list = new char *[strlen(file)];		// this is the worst case by far
		if (chinese)
			create_utf8_token_list(file, term_list);
		else
			{
			for (token = strtok(file, seperators); token != NULL; token = strtok(NULL, seperators))
				*current++ = token;
			*current = NULL;
			}


		for (first = term_list; *first != NULL; first++)
			{
//			fprintf(stderr, "%s\n", *first);
			where_to = buffer;
			for (last = first; *last != NULL; last++)
				{
				if (where_to == buffer)
					{
					strcpy(buffer, *first);
					where_to = buffer + strlen(buffer);
					if (chinese)
						{
						if ((*first[0] & 0x80) && ANT_parser::isutf8(*first))
							is_utf8_token = TRUE;
						else
							is_utf8_token = FALSE;
						}
					}
				else
					{
					if (!chinese)
//						{
//						if (!is_utf8_token && !ANT_parser::ischinese(*last))
//							*where_to++ = ' ';
//						}
//					else
						*where_to++ = ' ';
					strcpy(where_to, *last);
					where_to += strlen(*last);
					}

				*where_to = '\0';

//				for the possible debugging later
//				static char di[] = {(char)0xe6, (char)0xa2, (char)0x85};
//				static char xianjin[] = {(char)0xe3, (char)0x80, (char)0x8a, (char)0xe7, (char)0x8e, (char)0xb0};
//				if (memcmp(buffer, xianjin, 6) == 0)
//					fprintf(stderr, "I got you");
//				if (strncmp(*last, "\"", 1)) == 0)
//					fprintf(stderr, "I got you");

				index_term = find_term_in_list(buffer, link_index, terms_in_index);

				if (index_term == NULL)
					break;		// we're after the last term in the list so can stop because we can't be a substring

				if (chinese)
					{
					is_substring = FALSE;
					cmp = utf8_token_compare(buffer, index_term->term, &is_substring);
					}
				else
					cmp = string_compare(buffer, index_term->term);

				if (cmp == 0)		// we're a term in the list
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
					if (chinese)
						cmp = is_substring == TRUE ? 0 : 1;
					else
						cmp = memcmp(buffer, index_term->term, strlen(buffer));
					if  (cmp != 0)
						break;		// we're a not a substring so we can't find a longer term
					}
				}
			}
		if (chinese)
			free_utf8_token_list(term_list);
		delete [] term_list;
		delete [] file;

		if (file_number % 1000 == 0)
			fprintf(stderr, "Files processed:%d\n", file_number);
		file_number++;

		//filename = disk.get_next_filename();
		if (disk.next(&file_object) == NULL)
			file = filename = NULL;
		else
			{
			filename = file_object.filename;
			file = ANT_disk::read_entire_file(filename);
			}
		}
	}

print_answer(link_index, terms_in_index);

fprintf(stderr, "%s Completed\n", argv[0]);

return 0;
}

