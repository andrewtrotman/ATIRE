/*
	ZIPF_GRAPH.C
	------------
	Given a dictionary file (including cf and tf) produce the zipfian graph.
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../source/disk.h"

class ANT_dictionary
{
public:
	char *term;
	long df, cf;
public:
	static int cmp(const void *a, const void *b);
	static int find(const void *a, const void *b);
} ;

/*
	ANT_DICTIONARY::CMP()
	---------------------
*/
int ANT_dictionary::cmp(const void *a, const void *b)
{
ANT_dictionary *one = (ANT_dictionary *)a;
ANT_dictionary *two = (ANT_dictionary *)b;
int diff;

if ((diff = two->df - one->df) == 0)
	if ((diff = two->cf - one->cf) == 0)
		diff = strcmp(one->term, two->term);

return diff;
}

/*
	ANT_DICTIONARY::FIND()
	----------------------
*/
int ANT_dictionary::find(const void *a, const void *b)
{
ANT_dictionary *one = (ANT_dictionary *)a;
ANT_dictionary *two = (ANT_dictionary *)b;
int diff;

diff = strcmp(one->term, two->term);

return diff;
}

/*
	READ_FILE()
	-----------
*/
ANT_dictionary *read_file(char *filename, long long *length)
{
ANT_disk disk;
char *file, **line_list, **current, *pos;
long long lines;
ANT_dictionary *term_list, *term;

if ((file = disk.read_entire_file(filename)) == NULL)
	exit(printf("Cannot read input file:%s\n", filename));

line_list = disk.buffer_to_list(file, &lines);
term = term_list = new ANT_dictionary[(size_t)lines];
lines = 0;

for (current = line_list; *current != NULL; current++)
	{
	if (**current == '\0')		// ignore blank lines
		continue;
	lines++;
	sscanf(*current, "%*s %ld %ld", &term->cf, &term->df);
	term->term = *current;
	pos = strchr(*current, ' ');
	if (pos != NULL)
		*pos = '\0';		// '\0' terminate the string.
	term++;
	}
*length = lines;
return term_list;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
ANT_dictionary *dictionary, *terms, *hit;
long long dictionary_length, which, terms_length;

if (argc != 2 && argc != 3)
	exit(printf("Usage:%s <dictionary_file> [<termlist_file>]\n", argv[0]));

dictionary = read_file(argv[1], &dictionary_length);
if (argc == 2)
	{
	qsort(dictionary, (size_t)dictionary_length, sizeof(*dictionary), ANT_dictionary::cmp);
	for (which = 0; which < dictionary_length; which++)
		if (which % 100 == 0 || which <= 100)
			printf("%ld %ld\n", which, dictionary[which].df);
	}
else
	{
	terms = read_file(argv[2], &terms_length);
	qsort(dictionary, (size_t)dictionary_length, sizeof(*dictionary), ANT_dictionary::find);		// alphabetical order first
	for (which = 0; which < terms_length; which++)
		if ((hit = (ANT_dictionary *)bsearch(terms + which, dictionary, (size_t)dictionary_length, sizeof(*dictionary), ANT_dictionary::find)) != NULL)
			memcpy(terms + which, hit, sizeof(*hit));

	qsort(dictionary, (size_t)dictionary_length, sizeof(*dictionary), ANT_dictionary::cmp);		// term frequency order
	for (which = 0; which < terms_length; which++)
		if ((hit = (ANT_dictionary *)bsearch(terms + which, dictionary, (size_t)dictionary_length, sizeof(*dictionary), ANT_dictionary::cmp)) != NULL)
			printf("%d %d\n", hit - dictionary, hit->df);
	}

return 0;
}

