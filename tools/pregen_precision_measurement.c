#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>

#ifdef _MSC_VER

#include <windows.h>

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#endif

#include "../source/str.h"
#include "../source/string_pair.h"
#include "../source/file.h"
#include "../source/disk.h"
#include "../source/pregen.h"
#include "../source/search_engine_accumulator.h"
#include "../source/indexer_param_block_pregen.h"

#ifdef _MSC_VER
	char *map_entire_file(const char *filename, long long *filesize)
	{
	HANDLE mapping;
	HANDLE fp;
	char *result;

	(void) filesize; //TODO fill this parameter

	fp = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fp == INVALID_HANDLE_VALUE)
		return NULL;

	mapping = CreateFileMapping(fp, NULL, PAGE_READONLY, 0, 0, NULL);

	if (mapping == NULL)
		return NULL;

	result = (char*) MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);

	return result;
	}
#else
	char *map_entire_file(const char *filename, long long *filesize)
	{
	int fd = open(filename, O_RDONLY, (mode_t)0600);
	struct stat buffer;
	char *result;

	(void) filesize; //TODO fill this parameter

	if (fd == -1)
		return NULL;

	fstat(fd, &buffer);

	result = (char*) mmap (0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (result==MAP_FAILED)
		return NULL;

	return result;
	}
#endif

bool compare_rsv_greater(const std::pair<long long, pregen_t>& a, const std::pair<long long, pregen_t>& b)
{
//Tiebreak on docids
return a.second > b.second ? true : a.second == b.second ? (a.first < b.first) : false;
}

unsigned long long absll(signed long long a)
{
return a >= 0 ? a : -a;
}

void compare(ANT_pregen & f1, ANT_pregen & f2, int bits)
{
int print_rsvs = 0, print_ranks = 0, print_diffs = 0;

assert(f1.doc_count == f2.doc_count);
assert(bits <= sizeof(pregen_t) * CHAR_BIT);

/* Pregens give a score to each document, but we want to compare document ranks in the ordering */

std::pair<long long, pregen_t> *docs1 =	new std::pair<long long, pregen_t>[f1.doc_count], *docs2 = new std::pair<long long, pregen_t>[f2.doc_count];
long long *ranks1 = new long long[f1.doc_count], *ranks2 = new long long[f2.doc_count];

if (print_rsvs)
	{
	printf("RSVs:\n");
	printf("docid rsv1 rsv2\n");
	}

//Make an array of  docid -> (docid, score)
for (long long i = 0; i < f1.doc_count; i++)
	{
	docs1[i].first = i;
	docs1[i].second = f1.scores[i];
	docs2[i].first = i;

	/* If we need to mask off the lower bits of the pregen to get the desired number
	 * of bits, do that now.
	 */
	docs2[i].second = f2.scores[i] & ~(((pregen_t) 1 << (sizeof(pregen_t) * CHAR_BIT - bits)) - 1);

	if (print_rsvs)
		printf("%5lld %4llu %4llu\n", i, docs1[i].second, docs2[i].second);
	}

if (print_ranks)
	printf("\nRanks:\n");

//Sort to give an array of  rank -> (docid, score)
std::sort(docs1, docs1 + f1.doc_count, compare_rsv_greater);
std::sort(docs2, docs2 + f2.doc_count, compare_rsv_greater);

if (print_ranks)
	{
	printf("rank docid1 docid2\n");
	for (long long i = 0; i < f1.doc_count; i++)
		printf("%4lld %6lld %6lld\n", i, docs1[i].first, docs2[i].first);

	printf("\n");
	}

//Invert those to give an array of  docid -> rank
for (long long i = 0; i < f1.doc_count; i++)
	{
	ranks1[docs1[i].first] = i;
	ranks2[docs2[i].first] = i;
	}

//Now we can step through and compare the ranks in both lists
if (print_diffs)
	printf("docid rank1 rank2  diff\n");

unsigned long long sum_of_diffs = 0;

for (long long i = 0; i < f1.doc_count; i++)
	{
	long long diff = absll((signed long long) ranks1[i] - (signed long long) ranks2[i]);

	sum_of_diffs += diff;

	if (print_diffs)
		printf("%5lld %5lld %5lld %5lld\n", i, ranks1[i], ranks2[i], diff);
	}

fprintf(stderr, "%d, %llu, %llu, %.4f%%\n", bits, sum_of_diffs, sum_of_diffs / f1.doc_count, (double) sum_of_diffs / f1.doc_count / f1.doc_count * 100);
printf("%d, %llu, %llu, %.4f%%\n", bits, sum_of_diffs, sum_of_diffs / f1.doc_count, (double) sum_of_diffs / f1.doc_count / f1.doc_count * 100);

delete[] docs1;
delete[] docs2;
delete[] ranks1;
delete[] ranks2;
}

void compare_conflation(ANT_pregen & f1, ANT_pregen & f2, int bits)
{
assert(f1.doc_count == f2.doc_count);
assert(bits <= sizeof(pregen_t) * CHAR_BIT);

/* Pregens give a score to each document, but we want to compare document ranks in the ordering */

std::pair<long long, pregen_t> *docs1 =	new std::pair<long long, pregen_t>[f1.doc_count], *docs2 = new std::pair<long long, pregen_t>[f2.doc_count];
long long *ranks1 = new long long[f1.doc_count], *ranks2 = new long long[f2.doc_count];

//Make an array of  docid -> (docid, score)
for (long long i = 0; i < f1.doc_count; i++)
	{
	docs1[i].first = i;
	docs1[i].second = f1.scores[i];
	docs2[i].first = i;

	/* If we need to mask off the lower bits of the pregen to get the desired number
	 * of bits, do that now.
	 */
	docs2[i].second = f2.scores[i] & ~(((pregen_t) 1 << (sizeof(pregen_t) * CHAR_BIT - bits)) - 1);
	}

//Sort to give an array of  rank -> (docid, score)
std::sort(docs1, docs1 + f1.doc_count, compare_rsv_greater);
std::sort(docs2, docs2 + f2.doc_count, compare_rsv_greater);

/*
printf("rank docid1 rsv1 docid2 rsv2\n");
for (long long i = 0; i < f1.doc_count; i++)
	printf("%4lld %6lld %12llu %6lld %12llu\n", i, docs1[i].first, (unsigned long long) docs1[i].second, docs2[i].first, (unsigned long long) docs2[i].second);

printf("\n");
*/


long long i = 0, j = 0;
unsigned long long conflations = 0;

while (i < f1.doc_count)
	{
	/* We have a group of documents with the same score on the docs1 side, all of which
	 * should appear in the group of documents with the same score on the docs2 side. If the docs2
	 * group contains any extra documents, however, then those documents have been conflated, increasing
	 * our conflation error score.
	 */
	pregen_t docs1_group = docs1[i].second;
	pregen_t docs2_group = docs2[j].second;

	unsigned long long docs2_group_first_docid = docs2[j].first;

	int docs2_count = 0, docs2_unmatched_count;

	//Count up the members of the groups
	for (long long docs2_group_j = j; docs2[docs2_group_j].second == docs2_group; docs2_group_j++)
		docs2_count++;

	docs2_unmatched_count = docs2_count;

	/* First check that all of the docs in the doc1 group are in the doc2 group (this checks that the pregen
	 * is "reasonable")
	 */
	do
		{
		int found = 0;
		for (long long docs2_group_j = j; docs2_group_j < j + docs2_count; docs2_group_j++)
			if (docs1[i].first == docs2[docs2_group_j].first)
				{
				docs2_unmatched_count--;
				docs2[docs2_group_j].first = -1; //Mark as matched
				found = 1;
				break;
				}
		if (!found)
			{
			fprintf(stderr, "Pregen ordering is not reasonable. Document %llu is missorted, it incorrectly sorts later than document %llu\n", docs1[i].first, docs2_group_first_docid);
			exit(-1);
			}
		i++;
		}
	while (i < f1.doc_count && docs1[i].second == docs1_group);

	/* If the docs2 group still contains unmatched documents, they have been conflated. Assuming that the
	 * pregen is reasonable (doesn't misorder documents, though it may conflate them), they must be the
	 * documents of the subsequent group(s) in the docs1 array, so keep matching groups from docs1 against the group in docs2.
	 */
	if (docs2_unmatched_count > 0)
		{
		do
			{
			int found = 0;
			for (long long docs2_group_j = j; docs2_group_j < j + docs2_count; docs2_group_j++)
				if (docs1[i].first == docs2[docs2_group_j].first)
					{
					docs2_unmatched_count--;
					docs2[docs2_group_j].first = -1; //Mark as matched
					found = 1;
					break;
					}
			if (!found)
				{
				for (long long docs2_group_j = j; docs2_group_j < j + docs2_count; docs2_group_j++)
					if (docs2[docs2_group_j].first != -1)
						fprintf(stderr, "Pregen ordering is not reasonable. Document %llu (rank #%llu in pregen) is missorted, it incorrectly sorts before document %llu (rank #%llu in baseline)\n", docs2[docs2_group_j].first, docs2_group_j, docs1[i].first, i);
				exit(-1);
				}
			i++;
			}
		while (i < f1.doc_count && docs2_unmatched_count > 0);

		if (i < f1.doc_count && i > 0 && docs1[i-1].second == docs1[i].second)
			{
			fprintf(stderr, "Pregen ordering is not reasonable. Same-ranked documents %llu, %llu in doc1 get different ranks in doc2\n", docs1[i-1].first, docs1[i].first);
			exit(-1);
			}


		//All the documents in the docs2 group are counted as conflated.
		conflations += docs2_count;
		}

	//Advance to next group in docs2
	j += docs2_count;
	}

fprintf(stderr, "%d, %llu, %.4f%%\n", bits, conflations, (double) conflations / f1.doc_count * 100);
printf("%d, %llu, %.4f%%\n", bits, conflations, (double) conflations / f1.doc_count * 100);

delete[] docs1;
delete[] docs2;
delete[] ranks1;
delete[] ranks2;
}

int file_exists(const char *filename)
{
FILE *file = fopen(filename, "r");

if (file)
	{
	fclose(file);
	return 1;
	}
return 0;
}

void test(){
	ANT_string_pair test("river packs WOLF RP (accepting! New)");
	ANT_string_pair test2("");
	ANT_string_pair test3("=)");

	printf("%llu\n", ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(test));
	printf("%llu\n", ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(test2));
	printf("%llu\n", ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(test3));
}

int main(int argc, char ** argv)
{
	test();
ANT_indexer_param_block_pregen pregen_params;

int num_pregens = (argc - 2) / 2;
char **pregen_type = new char*[num_pregens];
char **pregen_filenames = new char*[num_pregens];

int already_have_pregen_files = 1;

assert(argc >= 3);

printf("Pregen field size is %d bytes\n\n", sizeof(pregen_t));

/*
 * Which pregens are we examining? Use the indexer_param_block_pregen class to parse from command line
 */
for (int i = 2, pregen_index = 0; i < argc; i += 2, pregen_index++)
	{
	char * field_name = argv[i];
	char * field_type = argv[i + 1];
	std::ostringstream filenamebuf;

	pregen_type[pregen_index] = field_type;

	filenamebuf << "pregen." << field_name << "." << field_type;

	pregen_filenames[pregen_index] = strnew(filenamebuf.str().c_str());

	if (!pregen_params.add_pregen_field(field_name, field_type))
		exit(printf("Unknown pregen field type '%s'\n", field_type));

	already_have_pregen_files = already_have_pregen_files && file_exists(pregen_filenames[pregen_index]);
	}

if (already_have_pregen_files)
	fprintf(stderr, "Pregen files already exist, reading those...\n");
else
	{
	ANT_pregens_writer pregen_writer;
	char *doclist, *cur;

	fprintf(stderr, "Generating pregens from doclist...\n");

	//Read document names from the .doclist file
	doclist = map_entire_file(argv[1], NULL);

	if (!doclist)
		{
		fprintf(stderr, "Couldn't read doclist\n");
		exit(-1);
		}

	//Create pregen field writers for each of the fields we're examining...
	for (int i = 0; i < pregen_params.num_pregen_fields; i++)
		if (!pregen_writer.add_field(pregen_filenames[i], pregen_params.pregens[i].field_name, pregen_params.pregens[i].type))
			exit(fprintf(stderr, "Couldn't open pregen file '%s'\n", pregen_filenames[i]));

	long long docindex = 0;

	cur = doclist;
	while (*cur)
		{
		char * docnameend = strchr(cur, '\n');

		if (!docnameend)
			break;

		pregen_writer.process_document(docindex, ANT_string_pair(cur, docnameend - cur));

		cur = docnameend + 1;
		docindex++;
		}

	/* Do we have a list of exact strings per-document to display for this pregen? */
	/*for (int i = 0; i < num_pregens; i++)
	 if (dynamic_cast<ANT_pregen_writer_exact_strings*>(pregen_writer.fields[i]))
	 {
	 ANT_pregen_writer_exact_strings* p = dynamic_cast<ANT_pregen_writer_exact_strings*>(pregen_writer.fields[i]);
	 exact_strings = new char*[p->doc_count];

	 for (int j = 0; j < p->doc_count; j++)
	 {
	 exact_strings[j] = strdup(p->exact_strings[j].second);
	 if (strlen(exact_strings[j])>20)
	 exact_strings[j][20] = 0;
	 }
	 }
	 */
	pregen_writer.close();
	}

/* Pregens are written to a file, so read the pregens that we just wrote back into memory */
ANT_pregen *pregens = new ANT_pregen[num_pregens];

for (int i = 0; i < num_pregens; i++)
	pregens[i].read(pregen_filenames[i]);

fprintf(stderr, "Comparing pregens...\n");

/* Compare pregens on the same fields so we can check relative effectiveness of different methods.
 *
 * We'll take the first appearance of a field's pregen as the baseline to compare the
 * rest of the results against
 */
int *compared = new int[num_pregens]; //Values are true if we've compared against this pregen already

memset((void *) compared, 0, sizeof(compared[0]) * num_pregens);

for (int i = 0; i < num_pregens; i++)
	if (!compared[i])
		{
		printf("Pregen on field: %s, method: %s\n", pregens[i].field_name, pregen_type_to_str(pregens[i].type));

		/* Do we have a list of exact strings per-document to display for this pregen? */
		/*if (dynamic_cast<ANT_pregen_writer_exact_strings*>(pregen_writer.fields[i]))
		 {
		 dynamic_cast<ANT_pregen_writer_exact_strings*>(pregen_writer.fields[i])->print_strings();
		 }*/

		for (int j = i + 1; j < num_pregens; j++)
			if (strcmp(pregens[i].field_name, pregens[j].field_name) == 0)
				{
				fprintf(stderr, "Comparing with: %s\n", pregen_type_to_str(pregens[j].type));
				printf("Comparing with: %s\n", pregen_type_to_str(pregens[j].type));
				printf("Sum of rank diffs, average, percentage\n");

				for (int bits = 64; bits >= 64; bits-=1)
					{
					compare_conflation(pregens[i], pregens[j], bits);
					}

				compared[j] = 1;
				}
		}

//Leak everything
return EXIT_SUCCESS;
}
