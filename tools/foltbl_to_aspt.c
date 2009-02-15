/*
	FOLTBL_TO_ASPT.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"
#include "../source/str.h"

char buffer[1024 * 1024];

/*
	class ANT_TAG
	-------------
*/
class ANT_tag
{
friend class ANT_tag_set;

private:
	char *name;	
	long occurrences;

private:
	ANT_tag(void) { name = NULL; }

public:
	ANT_tag(char *name);
	virtual ~ANT_tag();
	long add(void) { return ++occurrences; }
	long get_occurrences(void) { return occurrences; }
	static int cmp(const void *a, const void *b);
	void text_render(void) { printf("%s %d\n", name, occurrences); }
} ;

/*
	ANT_TAG::ANT_TAG()
	------------------
*/
ANT_tag::ANT_tag(char *name)
{
this->name = strnew(name);
occurrences = 0;
}

/*
	ANT_TAG::~ANT_TAG()
	-------------------
*/
ANT_tag::~ANT_tag()
{
delete [] name;
}

/*
	ANT_TAG::CMP()
	--------------
*/
int ANT_tag::cmp(const void *a, const void *b)
{
ANT_tag *one, *two;

one = *(ANT_tag **)a;
two = *(ANT_tag **)b;

return strcmp(one->name, two->name);
}

/*
	class ANT_TAG_SET
	-----------------
*/
class ANT_tag_set
{
public:
	ANT_tag **tag_list;
	long tag_list_length, tag_list_used, tag_list_allocation_size;

public:
	ANT_tag_set();
	~ANT_tag_set();
	long add(char *name);
	void text_render(void);
} ;

/*
	ANT_TAG_SET::ANT_TAG_SET()
	--------------------------
*/
ANT_tag_set::ANT_tag_set()
{
tag_list = NULL;
tag_list_used = 0;
tag_list_length = 0;
tag_list_allocation_size = 1024;
}

/*
	ANT_TAG_SET::~ANT_TAG_SET()
	---------------------------
*/
ANT_tag_set::~ANT_tag_set()
{
free(tag_list);
}

/*
	ANT_TAG_SET::ADD()
	------------------
*/
long ANT_tag_set::add(char *name)
{
ANT_tag key, *key_pointer, **got, *another;

key.name = name;
key_pointer = &key;

got = (ANT_tag **)bsearch(&key_pointer, tag_list, tag_list_used, sizeof(*tag_list), ANT_tag::cmp);
if (got == NULL)
	{
	another = new ANT_tag(name);
	got = &another;
	if (tag_list_used >= tag_list_length)
		tag_list = (ANT_tag **)realloc(tag_list, sizeof(*tag_list) * ((tag_list_length += tag_list_allocation_size) + 1));
	tag_list[tag_list_used] = another;
	tag_list_used++;
	tag_list[tag_list_used] = NULL;
	qsort(tag_list, tag_list_used, sizeof(*tag_list), ANT_tag::cmp);
	}

(*got)->add();
key.name = NULL;
return (*got)->get_occurrences();
}

/*
	ANT_TAG_SET::TEXT_RENDER()
	--------------------------
*/
void ANT_tag_set::text_render(void)
{
ANT_tag **current;

for (current = tag_list; *current != NULL; current++)
	(*current)->text_render();
}

/*
	CLEAN_STRING()
	--------------
*/
void clean_string(char *string)
{
long in;
char *ch;

in = false;
for (ch = string; *ch != '\0'; ch++)
	if (in)
		{
		if (*ch == ']')
			in = false;
		*ch = ' ';
		}
	else
		if (*ch == '[')
			{
			*ch = ' ';
			in = true;
			}
}

/*
	STR_STAR_CMP()
	--------------
*/
int str_star_cmp(const void *a, const void *b)
{
char *one, *two;

one = *(char **)a;
two = *(char **)b;

return strcmp(one, two);
}

/*
	GENERATE_UNIQUE_PATH_SET()
	--------------------------
*/
void generate_unique_path_set(char **path_list, long path_list_length)
{
char **all, **current, *from, *to, **into, *prev;

into = all = new char *[path_list_length + 1];
for (current = path_list; *current != NULL; current++)
	{
	if (**current == '\0')
		continue;
	if ((from = strchr(*current, ',')) == 0)
		exit(printf("Line %ld should contain at least one comma\n", current - path_list));
	from++;
	if ((to = strchr(from, ',')) == 0)
		exit(printf("Line %ld should contain at least two commas\n", current - path_list));
	*to = '\0';
	*into = from;
	into++;
	}
*into = NULL;
qsort(all, into-all, sizeof(*all), str_star_cmp);

prev = "1";
for (into = all; *into != NULL; into++)
	{
	if (strcmp(*into, prev) != 0)
		puts(*into);
	prev = *into;
	}
}

/*
	READ_FILE()
	-----------
*/
long read_file(char *filename)
{
ANT_tag_set tags;
const char *SEPERATORS = " /\\";
char *from, *to, *file, *token, **lines, **current;
long length;

file = ANT_disk::read_entire_file(filename);
lines = ANT_disk::buffer_to_list(file, &length);

generate_unique_path_set(lines, length);

for (current = lines; *current != NULL; current++)
	{
	if (**current == '\0')
		continue;
	if (strlen(*current) > sizeof(buffer) - 1)
		exit(printf("Line %ld exceeds max line length (%ld chars)\n", current - lines, sizeof(buffer)));
	if ((from = strchr(*current, ',')) == 0)
		exit(printf("Line %ld should contain at least one comma\n", current - lines));
	from++;
	if ((to = strchr(from, ',')) == 0)
		exit(printf("Line %ld should contain at least two commas\n", current - lines));
	strncpy(buffer, from, to - from);
	buffer[to - from] = '\0';
//	printf(">%s<\n", buffer);
	clean_string(buffer);
	for (token = strtok(buffer, SEPERATORS); token != NULL; token = strtok(NULL, SEPERATORS))
		tags.add(token);
	}
tags.text_render();

return 0;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{

if (argc != 2)
	exit(printf("Usage:%s <text_fol_table_file>\n", argv[0]));

read_file(argv[1]);
}
