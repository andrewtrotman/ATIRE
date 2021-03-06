/*
	ANT_DICTIONARY.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "maths.h"
#include "memory.h"
#include "search_engine.h"
#include "btree_iterator.h"
#include "search_engine_btree_leaf.h"
#include "phonetic_double_metaphone.h"
#include "phonetic_soundex.h"
#include "impact_header.h"

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
	long long docid, max_docid, sum;
	ANT_compressable_integer *current, *end;
	ANT_compressable_integer *impact_value_ptr, *doc_count_ptr, *impact_offset_ptr;
	ANT_compressable_integer *impact_offset_start;

	max_docid = sum = 0;
	impact_value_ptr = impact_header;
	doc_count_ptr = impact_header + quantum_count;
	impact_offset_start = impact_offset_ptr = impact_header + quantum_count * 2;

	while (doc_count_ptr < impact_offset_start)
		{
		factory->decompress(buffer, postings_list + *impact_offset_ptr, *doc_count_ptr);

		docid = -1;
		current = buffer;
		end = buffer + *doc_count_ptr;
		while (current < end)
			{
			docid += *current++;
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

		impact_value_ptr++;
		impact_offset_ptr++;
		doc_count_ptr++;
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
	ABOUT()
	-------
*/
void about(char *exename)
{
exit(printf("Usage:%s [index.aspt] [-s <start word> [-e <end word>]] [-d<oubleMetaphone>] [-x<soundex>] [-u<nicodeWideChars>] [-p<rintPostings>] [-q<uiet (without df and cf)>] [-l<PrintOnePostingPerLine>]\n", exename));
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
bool print_df_cf = true;
ANT_stem *meta = NULL;
ANT_compressable_integer *raw;
long long max = 0;
long long postings_list_size;
long long raw_list_size;
long long global_trim;
unsigned char *postings_list = NULL;
long param, filename = 0;
char *term, *first_term, *last_term;
ANT_search_engine_btree_leaf leaf;
ANT_compression_factory factory;
long metaphone, print_wide, print_postings, one_postings_per_line;
ANT_memory memory;
ANT_search_engine search_engine(&memory);

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
	else if (strcmp(argv[param], "-q") == 0)
		print_df_cf = FALSE;
	else if (strcmp(argv[param], "-l") == 0)
		one_postings_per_line = TRUE;
	else if (strcmp(argv[param], "-h") == 0)
		about(argv[0]);
	else if (strcmp(argv[param], "-?") == 0)
		about(argv[0]);
	else if (*argv[param] == '-')
		about(argv[0]);
	else
		{
		// already have been given an index filename
		if (filename != 0)
			about(argv[0]);
		filename = param;
		}
	}

search_engine.open(filename == 0 ? "index.aspt" : argv[filename]);

global_trim = search_engine.get_global_trim_postings_k();
ANT_btree_iterator iterator(&search_engine);

#ifdef IMPACT_HEADER
	quantum_count_type the_quantum_count;
	beginning_of_the_postings_type beginning_of_the_postings;
	long long impact_header_info_size = ANT_impact_header::INFO_SIZE;
	long long impact_header_size = ANT_impact_header::NUM_OF_QUANTUMS * sizeof(ANT_compressable_integer) * 3;
	ANT_compressable_integer *impact_header_buffer = (ANT_compressable_integer *)malloc(impact_header_size);
#endif

postings_list_size = search_engine.get_postings_buffer_length();
#ifdef IMPACT_HEADER
	raw_list_size = sizeof(*raw) * (search_engine.document_count() + ANT_COMPRESSION_FACTORY_END_PADDING);
#else
	raw_list_size = sizeof(*raw) * (512 + search_engine.document_count() + ANT_COMPRESSION_FACTORY_END_PADDING);
#endif

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

		if (print_df_cf)
			{
			#ifdef SPECIAL_COMPRESSION
				if (leaf.local_document_frequency < 3)
					printf("%lld %lld 0", leaf.local_collection_frequency, leaf.local_document_frequency);
				else
			#endif
			printf("%lld %lld %lld", leaf.local_collection_frequency, leaf.local_document_frequency, leaf.postings_length);
			}

		if (check_postings && *term != '~')		// ~length and others aren't encoded in the usual way
			{
			postings_list = search_engine.get_postings(&leaf, postings_list);

#ifdef IMPACT_HEADER
			// decompress the header
			the_quantum_count = ANT_impact_header::get_quantum_count(postings_list);
			beginning_of_the_postings = ANT_impact_header::get_beginning_of_the_postings(postings_list);
			factory.decompress(impact_header_buffer, postings_list + impact_header_info_size, the_quantum_count * 3);

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
