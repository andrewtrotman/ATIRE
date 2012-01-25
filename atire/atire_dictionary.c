/*
	ANT_DICTIONARY.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/maths.h"
#include "../source/memory.h"
#include "../source/search_engine.h"
#include "../source/btree_iterator.h"
#include "../source/search_engine_btree_leaf.h"
#include "../source/phonetic_double_metaphone.h"
#include "../source/phonetic_soundex.h"
#include "../source/impact_header.h"

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


#ifdef IMPACT_HEADER
	/*
		PROCESS()
		---------
	*/
	long long process(ANT_compression_factory *factory, uint32_t quantum_count, ANT_compressable_integer *impact_header, ANT_compressable_integer *buffer, unsigned char *postings_list, long long trim_point, long verbose, long one_postings_per_line)
	{
	ANT_compressable_integer tf;
	long long docid, max_docid, sum;
	ANT_compressable_integer *current, *end, *end_offset;
	ANT_compressable_integer *impact_value_ptr, *doc_count_ptr, *impact_offset_ptr;

	max_docid = sum = 0;
	impact_value_ptr = impact_header;
	doc_count_ptr = impact_header + quantum_count;
	end_offset = impact_header + quantum_count * 3;

	for (impact_offset_ptr = (impact_header + quantum_count * 2); impact_offset_ptr < end_offset; impact_value_ptr++, doc_count_ptr++, impact_offset_ptr++)
		{
		//printf("*doc_count: %lld, impact_offset: %lld\n", (long long)*doc_count_ptr, (long long)*impact_offset_ptr);
		factory->decompress(buffer, postings_list + *impact_offset_ptr, *doc_count_ptr);

		end = buffer + *doc_count_ptr;
		for (current = buffer, docid = -1; current < end; current++)
			{
			docid += *current;
			if (verbose)
				if (one_postings_per_line)
					printf("\n<%lld,%lld>", docid, (long long)*impact_value_ptr);
				else
					printf("<%lld,%lld>", docid, (long long)*impact_value_ptr);

			if (docid > max_docid)
				max_docid = docid;
			}

		sum += *doc_count_ptr;
		if (sum >= trim_point) 
			break;
		}

	return max_docid;
	}
#else
	/*
		PROCESS()
		---------
	*/
	long long process(ANT_compressable_integer *impact_ordering, long long document_frequency, long verbose, long one_postings_per_line)
	{
	ANT_compressable_integer tf;
	long long docid, max;
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
				if (one_postings_per_line)
					printf("\n<%lld,%lld>", docid, (long long)tf);
				else
					printf("<%lld,%lld>", docid, (long long)tf);
			}
		if (docid > max)
			max = docid;
		current++;						// zero
		}

	return max;
	}
#endif

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stem *meta = NULL;
ANT_compressable_integer *raw;
long long max = 0;
long long postings_list_size = 100 * 1024 * 1024;
long long raw_list_size = 100 * 1024 * 1024;
unsigned char *postings_list = NULL;
char *term, *first_term, *last_term;
ANT_memory memory;
ANT_search_engine search_engine(&memory);
search_engine.open();
long long global_trim = search_engine.get_global_trim_postings_k();
ANT_btree_iterator iterator(&search_engine);
ANT_search_engine_btree_leaf leaf;
ANT_compression_factory factory;
long metaphone, print_wide, print_postings, one_postings_per_line;
long param;

#ifdef IMPACT_HEADER
	uint32_t the_quantum_count;
	uint32_t beginning_of_the_postings;
	long long impact_header_info_size = ANT_impact_header::INFO_SIZE;
	long long impact_header_size = ANT_impact_header::NUM_OF_QUANTUMS * sizeof(ANT_compressable_integer) * 3;
	ANT_compressable_integer *impact_header_buffer = (ANT_compressable_integer *)malloc(impact_header_size);
#endif

first_term = last_term = NULL;
print_postings = print_wide = metaphone = one_postings_per_line = FALSE;

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
	else if (strcmp(argv[param], "-l") == 0)
		one_postings_per_line = TRUE;
	else
		exit(printf("Usage:%s [-s <start word> [-e <end word>]] [-d<oubleMetaphone>] [-x<soundex>] [-u<nicodeWideChars>] [-p<rintPostings>] [-l<PrintOnePostingPerLine>]\n", argv[0]));
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
		printf("%lld %lld", leaf.local_collection_frequency, leaf.local_document_frequency);
		if (check_postings && *term != '~')		// ~length and others aren't encoded in the usual way
			{
			if (leaf.local_document_frequency > 2)
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
#ifdef IMPACT_HEADER
			// decompress the header
			// the first 8 bytes are for postings_chain and second 8 bytes are for chain_length
			the_quantum_count = ((uint32_t *)postings_list)[4];
			beginning_of_the_postings = ((uint32_t *)postings_list)[5];
			factory.decompress(impact_header_buffer, postings_list+impact_header_info_size, the_quantum_count*3);

			// print the postings
			max = process(&factory, the_quantum_count, impact_header_buffer, raw, postings_list + beginning_of_the_postings, ANT_min(leaf.local_document_frequency, global_trim), print_postings, one_postings_per_line);

#else
			factory.decompress(raw, postings_list, leaf.impacted_length);
			max = process((ANT_compressable_integer *)raw, ANT_min(leaf.local_document_frequency, global_trim), print_postings, one_postings_per_line);
#endif

			if (max > search_engine.document_count())
				printf(" Largest Docno:%lld MAX IS TOO LARGE!", (long long)max);
			}
		putchar('\n');
		}
	}

return 0;
}