#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>

#include "../source/str.h"
#include "../source/file.h"
#include "../source/disk.h"
#include "../source/pregen.h"
#include "../source/search_engine_accumulator.h"
#include "../source/indexer_param_block_pregen.h"

char *pregen_type_to_str(pregen_field_type type) {
	switch (type)
		{
		case INTEGER:
			return "integer";
		case STRTRUNC:
			return "strtrunc";
		case ASCIIDIGEST:
			return "asciidigest";
		case BASE36:
			return "base36";
		case RECENTDATE:
			return "recentdate";
		case INTEGEREXACT:
			return "integerexact";
		case STREXACT:
			return "strexact";
		default:
			return "Unknown";
		};
}

bool compare_rsv_greater(const std::pair<long long, long long>& a, const std::pair<long long, long long>& b)
{
//Tiebreak on docids
return a.second > b.second ? true :
		a.second == b.second ? (a.first < b.first) : false;
}

void compare(ANT_pregen & f1, ANT_pregen & f2)
{
printf("Comparing with: %s\n", pregen_type_to_str(f2.type));

assert(f1.doc_count == f2.doc_count);

/* Pregens give a score to each document, but we want to compare document ranks in the ordering */

std::pair<long long, pregen_t> docs1[f1.doc_count], docs2[f2.doc_count];
long long ranks1[f1.doc_count], ranks2[f2.doc_count];

printf("RSVs:\n");
printf("docid rsv1 rsv2\n");

//Make an array of  docid -> (docid, score)
for (long long i = 0; i < f1.doc_count; i++)
	{
	docs1[i].first = i;
	docs1[i].second = f1.scores[i];
	docs2[i].first = i;
	docs2[i].second = f2.scores[i];
	printf("%5lld %4lld %4lld\n", i, docs1[i].second, docs2[i].second);
	}

printf("\nRanks:\n");

//Sort to give an array of  rank -> (docid, score)
std::sort(docs1, docs1 + f1.doc_count, compare_rsv_greater);
std::sort(docs2, docs2 + f2.doc_count, compare_rsv_greater);

printf("rank docid1 docid2\n");
for (long long i = 0; i < f1.doc_count; i++)
	printf("%4lld %6lld %6lld\n", i, docs1[i].first, docs2[i].first);

printf("\n");

//Invert those to give an array of  docid -> rank
for (long long i = 0; i < f1.doc_count; i++)
	{
	ranks1[docs1[i].first] = i;
	ranks2[docs2[i].first] = i;
	}

//Now we can step through and compare the ranks in both lists
printf("docid rank1 rank2  diff\n");
for (long long i = 0; i < f1.doc_count; i++)
	printf("%5lld %5lld %5lld %5d\n", i, ranks1[i], ranks2[i], abs((signed long long) ranks1[i] - (signed long long) ranks2[i]));

printf("\n");
}

int main(int argc, char ** argv)
{
char *doclist, *cur;
std::vector<char *> docnames;
ANT_indexer_param_block_pregen pregen_params;
ANT_pregens_writer pregen_writer;

int num_pregens = (argc - 2) / 2;
char * pregen_type[num_pregens];
char *pregen_filenames[num_pregens];

assert(argc >= 3);

printf("Pregen field size is %d bytes\n\n", sizeof(pregen_t));

//Read document names from the .doclist file
doclist = ANT_disk::read_entire_file(argv[1], NULL);

cur = doclist;

while (*cur)
	{
	char * docnameend = strchr(cur, '\n');

	if (!docnameend)
		break;

	*docnameend = '\0';

	docnames.push_back(cur);
	cur = docnameend + 1;
	}

/*
 * Which pregens are we examining? Use the indexer_param_block_pregen class to parse from command line
 */
for (int i = 2, pregen_index = 0; i < argc; i += 2, pregen_index++)
	{
	char * field_name = argv[i];
	char * field_type = argv[i + 1];

	pregen_type[pregen_index] = field_type;

	if (!pregen_params.add_pregen_field(field_name, field_type))
		exit(printf("Unknown pregen field type '%s'\n", field_type));
	}

for (int i = 0; i < pregen_params.num_pregen_fields; i++)
	{
	std::ostringstream filenamebuf;

	filenamebuf << "pregen." << pregen_params.pregens[i].field_name << "." << pregen_type[i];

	pregen_filenames[i] = strnew(filenamebuf.str().c_str());

	if (!pregen_writer.add_field(pregen_filenames[i], pregen_params.pregens[i].field_name, pregen_params.pregens[i].type))
		exit(printf("Couldn't open pregen file '%s'\n", pregen_filenames[i]));
	}

for (int i = 0; i < docnames.size(); i++)
	pregen_writer.process_document(i, docnames[i]);

pregen_writer.close();

/* Pregens are written to a file, so read the pregens that we just wrote back into memory */
ANT_pregen pregens[pregen_writer.field_count];

for (int i = 0; i < pregen_writer.field_count; i++)
	{
	pregens[i].read(pregen_filenames[i]);
	}

/* Compare pregens on the same fields so we can check relative effectiveness of different methods */

int compared[num_pregens]; //Values are true if we've compared against this pregen already

memset((void *)&compared[0], 0, sizeof(compared[0])*num_pregens);

/* We'll take the first appearance of a field's pregen as the baseline to compare the
 * rest of the results against
 */
for (int i = 0; i < num_pregens; i++)
	if (!compared[i])
		{
		printf("Pregen on field: %s, method: %s\n", pregens[i].field_name, pregen_type_to_str(pregens[i].type));

		/* Do we have a list of exact strings per-document to display for this pregen? */
		if (dynamic_cast<ANT_pregen_writer_exact_strings*>(pregen_writer.fields[i]))
			{
			dynamic_cast<ANT_pregen_writer_exact_strings*>(pregen_writer.fields[i])->print_strings();
			}

		for (int j = i + 1; j < num_pregens; j++)
			if (strcmp(pregens[i].field_name, pregens[j].field_name)==0)
				{
				compare(pregens[i], pregens[j]);

				compared[j] = 1;
				}
		}

//Leak everything
return EXIT_SUCCESS;
}
