/*
	FOLTBL_TO_ASPT.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"
#include "../source/str.h"
#include "../source/ctypes.h"

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
	bool memory_owner;

private:
	ANT_tag(void) { name = NULL; memory_owner = false; }

public:
	ANT_tag(char *name, long occ = 0);
	virtual ~ANT_tag();
	long add(void) { return ++occurrences; }
	long get_occurrences(void) { return occurrences; }
	static int cmp(const void *a, const void *b);
	void text_render(void) { printf("%s %d\n", name, occurrences); }
	void text_render(FILE *outfile) { fwrite(name, strlen(name) + 1, 1, outfile); }
} ;

/*
	ANT_TAG::ANT_TAG()
	------------------
*/
ANT_tag::ANT_tag(char *name, long occ)
{
this->name = strnew(name);
this->occurrences = occ;
memory_owner = true;
}

/*
	ANT_TAG::~ANT_TAG()
	-------------------
*/
ANT_tag::~ANT_tag()
{
if (memory_owner)
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
	ANT_tag *push(char *name, long occurrences);		// this appends and does not sort.
	void sort(void);
	ANT_tag **find(char *name) { return find(name, tag_list_used); }
	ANT_tag **find(char *name, long fake_length);
	long add(char *name);							// add and qsort (a very crude insertion sort).
	void include(ANT_tag_set *me);
	void text_render(void);
	void text_render(FILE *outfile);
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
long current;

for (current = 0; current < tag_list_used; current++)
	delete tag_list[current];
free(tag_list);
}

/*
	ANT_TAG_SET::PUSH()
	-------------------
*/
ANT_tag *ANT_tag_set::push(char *name, long occ)
{
ANT_tag *another;

another = new ANT_tag(name, occ);

if (tag_list_used >= tag_list_length)
	tag_list = (ANT_tag **)realloc(tag_list, sizeof(*tag_list) * ((tag_list_length += tag_list_allocation_size) + 1));
tag_list[tag_list_used] = another;
tag_list_used++;
tag_list[tag_list_used] = NULL;

return another;
}

/*
	ANT_TAG_SET::SORT()
	-------------------
*/
void ANT_tag_set::sort(void)
{
qsort(tag_list, tag_list_used, sizeof(*tag_list), ANT_tag::cmp);
}

/*
	ANT_TAG_SET::INCLUDE()
	----------------------
*/
void ANT_tag_set::include(ANT_tag_set *with)
{
long current;
long fake_length;
ANT_tag **into;

fake_length = tag_list_used;
for (current = 0; current < with->tag_list_used; current++)
	if ((into = find(with->tag_list[current]->name, fake_length)) == NULL)
		push(with->tag_list[current]->name, with->tag_list[current]->occurrences);
	else
		(*into)->occurrences += with->tag_list[current]->occurrences;

sort();
}

/*
	ANT_TAG_SET::FIND()
	-------------------
*/
ANT_tag **ANT_tag_set::find(char *name, long fake_length)
{
ANT_tag key, *key_star;
void *got;

key.name = name;
key_star = &key;
got = bsearch(&key_star, tag_list, fake_length, sizeof(*tag_list), ANT_tag::cmp);

if (got == NULL)
	return NULL;

return (ANT_tag **)got;
}


/*
	ANT_TAG_SET::ADD()
	------------------
*/
long ANT_tag_set::add(char *name)
{
ANT_tag key, *key_pointer, **got;

key.name = name;
key_pointer = &key;

got = (ANT_tag **)bsearch(&key_pointer, tag_list, tag_list_used, sizeof(*tag_list), ANT_tag::cmp);
if (got == NULL)
	{
	key_pointer = push(name, 0);
	got = &key_pointer;
	sort();
	}

(*got)->add();
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
	ANT_TAG_SET::TEXT_RENDER()
	--------------------------
*/
void ANT_tag_set::text_render(FILE *outfile)
{
ANT_tag **current;

fwrite(&tag_list_used, sizeof(tag_list_used), 1, outfile);
for (current = tag_list; *current != NULL; current++)
	(*current)->text_render(outfile);
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
void generate_unique_path_set(ANT_tag_set *paths, char **path_list, long path_list_length)
{
char **all, **current, *from, *to, **into, *prev;
long occurrences;

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
qsort(all, into - all, sizeof(*all), str_star_cmp);

prev = *all;
occurrences = 1;
for (into = all + 1; *into != NULL; into++)
	{
	if (strcmp(*into, prev) == 0)
		occurrences++;
	else
		{
		paths->push(prev, occurrences);
		prev = *into;
		occurrences = 1;
		}
	}
paths->push(prev, occurrences);
paths->sort();
}

/*
	READ_FILE()
	-----------
*/
ANT_tag_set *read_file(char *filename)
{
ANT_tag_set *paths;
char *file, **lines;
long long pos, file_length;
long length;

paths = new ANT_tag_set;

file = ANT_disk::read_entire_file(filename, &file_length);

for (pos = 0; pos < file_length; pos++)		// yea, right, so the file has '\0' characters in it!
	if (file[pos] == 0)
		file[pos] = ' ';

lines = ANT_disk::buffer_to_list(file, &length);
generate_unique_path_set(paths, lines, length);

delete [] file;
delete [] lines;

return paths;
}

/*
	MAKE_FILE_BINARY()
	------------------
*/
void make_file_binary(char *filename, ANT_tag_set *pathlist, FILE *outfile)
{
char *file, **lines, **current;
long length;
long long file_length, pos;
char *path_start, *path_end, *from;
long rel_file, rel_offset, rel_length, rel_path;
ANT_tag **got;

file = ANT_disk::read_entire_file(filename, &file_length);
for (pos = 0; pos < file_length; pos++)		// yea, right, so the file has '\0' characters in it!
	if (file[pos] == 0)
		file[pos] = ' ';

lines = ANT_disk::buffer_to_list(file, &length);

for (current = lines; *current != NULL; current++)
	{
	if (**current == '\0')
		continue;
	rel_file = atol(*current);
	path_start = strchr(*current, ',') + 1;

	path_end = from = strchr(path_start, ',') + 1;
	rel_offset = atol(from);

	from = strchr(from, ',');
	rel_length = atol(from);

	*(path_end - 1) = '\0';

	got = pathlist->find(path_start);
	if (got == NULL)
		fprintf(stderr, "Path list missing path '%s'\n", path_start);
	else
		{
		rel_path = got - pathlist->tag_list;
		fwrite(&rel_file, sizeof(rel_file), 1, outfile);
		fwrite(&rel_path, sizeof(rel_path), 1, outfile);
		fwrite(&rel_offset, sizeof(rel_offset), 1, outfile);
		fwrite(&rel_length, sizeof(rel_length), 1, outfile);
		}
	}

delete [] file;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_tag_set all, *current;
long param;

if (argc < 2)
	exit(printf("Usage:%s <text_fol_table_file> ...\n", argv[0]));

fprintf(stderr, "Process input files\n");
for (param = 1; param < argc; param++)
	{
	fprintf(stderr, "%s\n", argv[param]);
	current = read_file(argv[param]);
	all.include(current);
	delete current;
	}
//all.text_render();

FILE *outfile = fopen("outfile", "wb");
all.text_render(outfile);
for (param = 1; param < argc; param++)
	{
	fprintf(stderr, "%s\n", argv[param]);
	make_file_binary(argv[param], &all, outfile);
	}
fclose(outfile);
}
