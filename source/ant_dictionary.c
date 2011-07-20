/*
	ANT_DICTIONARY.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "memory.h"
#include "search_engine.h"
#include "btree_iterator.h"
#include "search_engine_btree_leaf.h"
#include "phonetic_double_metaphone.h"
#include "phonetic_soundex.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifdef _MSC_VER
	#include <windows.h>
	wchar_t wide[10 * 1024];	// buffer for storing wide character strings before printing them
#endif
char metaphone_buffer[1024];	// buffer for storing the metaphone version of the string

int check_postings = 1;

/*
	PROCESS()
	---------
*/
long process(ANT_compressable_integer *impact_ordering, size_t document_frequency, long verbose)
{
ANT_compressable_integer tf;
long docid, max;
ANT_compressable_integer *current, *end;

max = 0;
current = impact_ordering;
end = impact_ordering + document_frequency;

while (current < end)
	{
	end += 2;
	docid = -1;
	tf = *current++;
	while (*current != 0)
		{
		docid += *current++;
		if (verbose)
			printf("<%lld,%lld>", (long long)docid, (long long)tf);
		}
	if (docid > max)
		max = docid;
	current++;						// zero
	}

return max;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stem *meta = NULL;
ANT_compressable_integer *raw, max = 0;
long long postings_list_size = 100 * 1024 * 1024;
long long raw_list_size = 100 * 1024 * 1024;
unsigned char *postings_list = NULL;
char *term, *first_term, *last_term;
ANT_memory memory;
ANT_search_engine search_engine(&memory);
search_engine.open();
ANT_btree_iterator iterator(&search_engine);
ANT_search_engine_btree_leaf leaf;
ANT_compression_factory factory;
long metaphone, print_wide, print_postings;
long param;

first_term = last_term = NULL;
print_postings = print_wide = metaphone = FALSE;

for (param = 1; param < argc; param++)
	{
	if (strcmp(argv[param], "-s") == 0)
		first_term = argv[++param];
	else if (strcmp(argv[param], "-e") == 0)
		last_term = argv[++param];
	else if (strcmp(argv[param], "-d") == 0)
		{
		metaphone = TRUE;
		meta = new ANT_phonetic_double_metaphone;
		}
	else if (strcmp(argv[param], "-x") == 0)
		{
		metaphone = TRUE;
		meta = new ANT_phonetic_soundex;
		}
	else if (strcmp(argv[param], "-u") == 0)
		print_wide = TRUE;
	else if (strcmp(argv[param], "-p") == 0)
		print_postings = TRUE;
	else
		exit(printf("Usage:%s [-s <start word> [-e <end word>]] [-d<oubleMetaphone>] [-x<sounded>] [-u<nicodeWideChars>] [-p<rintPostings>]\n", argv[0]));
	}

postings_list = (unsigned char *)malloc((size_t)postings_list_size);
raw = (ANT_compressable_integer *)malloc((size_t)raw_list_size);
for (term = iterator.first(first_term); term != NULL; term = iterator.next())
	{
	iterator.get_postings_details(&leaf);
	if (last_term != NULL && strcmp(last_term, term) < 0)
		break;
	else
		{
		if (metaphone)
			{
			if (isalpha(*term))
				meta->stem(term, metaphone_buffer);
			else
				strcpy(metaphone_buffer, "-");
			printf("%s ", metaphone_buffer);
			}
#ifdef _MSC_VER
		/*
			Convert into a wide string and print that as Windows printf() doesn't do UTF-8
		*/
		if (print_wide)
			if (MultiByteToWideChar(CP_UTF8, 0, term, -1, wide, sizeof(wide)) == 0)
				printf("FAIL ");
			else
				wprintf(L"%s ", wide);
		else
			printf("%s ", term);
#else
		printf("%s ", term);
#endif
		printf("%lld %d", leaf.collection_frequency, leaf.document_frequency);
		if (check_postings && *term != '~')		// ~length and others aren't encoded in the usual way
			{
			if (leaf.document_frequency > 2)
				if (leaf.postings_length > postings_list_size)
					{
					postings_list_size = 2 * leaf.postings_length;
					postings_list = (unsigned char *)realloc(postings_list, (size_t)postings_list_size);
					}
			postings_list = search_engine.get_postings(&leaf, postings_list);
			if (leaf.impacted_length > raw_list_size)
				{
				raw_list_size = 2 * leaf.impacted_length;
				raw = (ANT_compressable_integer *)realloc(raw, (size_t)raw_list_size);
				}
			factory.decompress(raw, postings_list, leaf.impacted_length);
			max = process((ANT_compressable_integer *)raw, leaf.document_frequency, print_postings);

			if (max > search_engine.document_count())
				printf(" Largest Docno:%lld MAX IS TOO LARGE!", (long long)max);
			}
		putchar('\n');
		}
	}

return 0;
}
