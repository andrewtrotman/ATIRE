/*
	ANT_DICTIONARY.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "search_engine.h"
#include "btree_iterator.h"
#include "search_engine_btree_leaf.h"
#ifdef _MSC_VER
	#include <windows.h>
	wchar_t wide[10 * 1024];	// buffer for storing wide character strings before printing them
#endif

int check_postings = 1;

/*
	PROCESS()
	---------
*/
long process(ANT_compressable_integer *impact_ordering, size_t document_frequency)
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
//		printf("<%lld,%lld>", (long long)tf, (long long)docid);
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
ANT_compressable_integer *raw, max = 0;
long postings_list_size = 100 * 1024 * 1024;
long raw_list_size = 100 * 1024 * 1024;
unsigned char *postings_list = NULL;
char *term, *first_term, *last_term;
ANT_memory memory;
ANT_search_engine search_engine(&memory);
ANT_btree_iterator iterator(&search_engine);
ANT_search_engine_btree_leaf leaf;
ANT_compression_factory factory;

if (argc == 1)
	first_term = last_term = NULL;
else if (argc == 2)
	{
	first_term = argv[1];
	last_term = NULL;
	}
else if (argc == 3)
	{
	first_term = argv[1];
	last_term = argv[2];
	}
else
	exit(printf("Usage:%s [<start word> [<end word>]]\n", argv[0]));

postings_list = (unsigned char *)malloc(postings_list_size);
raw = (ANT_compressable_integer *)malloc(raw_list_size);
for (term = iterator.first(first_term); term != NULL; term = iterator.next())
	{
	iterator.get_postings_details(&leaf);
	if (last_term != NULL && strcmp(last_term, term) < 0)
		break;
	else
		{
#ifdef _MSC_VER
		/*
			Convert into a wide string and print that as Windows printf() doesn't do UTF-8
		*/
		if (MultiByteToWideChar(CP_UTF8, 0, term, -1, wide, sizeof(wide)) == 0)
			printf("FAIL ");
		else
			wprintf(L"%s ", wide);
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
					postings_list = (unsigned char *)realloc(postings_list, postings_list_size);
					}
			search_engine.get_postings(&leaf, postings_list);
			if (leaf.impacted_length > raw_list_size)
				{
				raw_list_size = 2 * leaf.impacted_length;
				raw = (ANT_compressable_integer *)realloc(raw, raw_list_size);
				}
			factory.decompress(raw, postings_list, leaf.impacted_length);
			max = process((ANT_compressable_integer *)raw, leaf.document_frequency);

			if (max > search_engine.document_count())
				printf(" Largest Docno:%lld MAX IS TOO LARGE!", (long long)max);
			}
		putchar('\n');
		}
	}

return 0;
}
