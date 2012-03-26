/*
	PREGEN_EXAMINE.C
	----------------
*/
#include <cstdio>
#include <cassert>
#include <utility>
#include <algorithm>
#include <vector>
#include <limits.h>

#include "../source/pregen.h"
#include "../source/str.h"

/*
	COMPARE_RSV_GREATER()
	---------------------
*/
bool compare_rsv_greater(const std::pair<long long, ANT_pregen_t>& a, const std::pair<long long, ANT_pregen_t>& b)
{
//Tiebreak on docids
return a.second > b.second ? true : a.second == b.second ? (a.first < b.first) : false;
}

/*
	PRINT_PREGEN()
	--------------
*/
void print_pregen(std::vector<char *> &docnames, ANT_pregen &pregen, int bits)
{
assert(bits <= sizeof(ANT_pregen_t) * CHAR_BIT);

std::pair<long long, ANT_pregen_t> *docs1 =	new std::pair<long long, ANT_pregen_t>[pregen.doc_count];

//Make an array of  docid -> (docid, score)
for (long long i = 0; i < pregen.doc_count; i++)
	{
	docs1[i].first = i;

	/*
	 	 If we need to mask off the lower bits of the pregen to get the desired number
	 	 of bits, do that now.
	 */
	docs1[i].second = pregen.scores[i] & ~(((ANT_pregen_t) 1 << (sizeof(ANT_pregen_t) * CHAR_BIT - bits)) - 1);
	}

//Sort to give an array of  rank -> (docid, score)
std::sort(docs1, docs1 + pregen.doc_count, compare_rsv_greater);

printf("    rank    docid                  rsv title\n");
for (long long i = 0; i < pregen.doc_count; i++)
	printf("%8lld %8llu %20llu %s\n", i, docs1[i].first, docs1[i].second, docnames[docs1[i].first]);

printf("\n");

delete [] docs1;
}

/*
	MAIN()
	------
*/
int main(int argc, char **argv)
{
int argpos, docnames_mode;
char *doclist_fn;
char *empty_string = "";
FILE *doclist;
std::vector<char*> docnames;
char line[4096];

if (argc < 2)
	{
	printf("Usage: %s [-doclist <doclistfile>] <pregenfile> ...\n", argv[0]);
	return -1;
	}

if (strcmp(argv[1], "-doclist") == 0)
	{
	doclist_fn = argv[2];

	doclist = fopen(doclist_fn, "rb");

	if (doclist == NULL)
		{
		fprintf(stderr, "Couldn't load doclist file '%s'.\n", argv[1]);
		exit(-1);
		}

	fprintf(stderr, "Loading doclist...\n");

	while (fgets(line, sizeof(line), doclist) != NULL)
		{
		char *titlestart, *titleend;
		char *docnamestart = line;

		titlestart = strstr(docnamestart, "<title>");

		if (titlestart == NULL)
			{
			docnames.push_back(empty_string);
			continue;
			}

		titlestart += strlen("<title>");

		titleend = strstr(titlestart, "</title>");

		if (titleend == NULL)
			{
			docnames.push_back(empty_string);
			continue;
			}

		docnames.push_back(strnnew(titlestart, titleend - titlestart));
		}

	fclose(doclist);

	fprintf(stderr, "Doclist contains %llu documents.\n", (long long unsigned) docnames.size());

	docnames_mode = 1;
	argpos = 3;
	}
else
	{
	docnames_mode = 0;
	argpos = 1;
	}


fprintf(stderr, "Printing pregens...\n");

for (int i = argpos; i < argc; i++)
	{
	ANT_pregen pregen;

	printf("Pregen on field %s of type %s:\n", pregen.field_name, ANT_pregen::pregen_type_to_str(pregen.type));

	pregen.read(argv[i]);

	if (docnames_mode)
		{
		if (pregen.doc_count != docnames.size())
			printf("Pregen doc count %llu doesn't match doclist count %llu\n", pregen.doc_count, docnames.size());
		else
			print_pregen(docnames, pregen, 64);
		}
	else
		for (unsigned int i = 0; i < pregen.doc_count; i++)
			printf("%llu\n", (long long unsigned) pregen.scores[i]);
	}

//Leak the whole doclist
return 0;
}
