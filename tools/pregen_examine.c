#include <cstdio>
#include <cassert>
#include <utility>
#include <algorithm>
#include <vector>

#include "../source/pregen.h"

bool compare_rsv_greater(const std::pair<long long, pregen_t>& a, const std::pair<long long, pregen_t>& b)
{
//Tiebreak on docids
return a.second > b.second ? true : a.second == b.second ? (a.first < b.first) : false;
}

void print_pregen(std::vector<char *> &docnames, ANT_pregen &pregen, int bits)
{
assert(bits <= sizeof(pregen_t) * CHAR_BIT);

std::pair<long long, pregen_t> *docs1 =	new std::pair<long long, pregen_t>[pregen.doc_count];

//Make an array of  docid -> (docid, score)
for (long long i = 0; i < pregen.doc_count; i++)
	{
	docs1[i].first = i;

	/*
	 	 If we need to mask off the lower bits of the pregen to get the desired number
	 	 of bits, do that now.
	 */
	docs1[i].second = pregen.scores[i] & ~(((pregen_t) 1 << (sizeof(pregen_t) * CHAR_BIT - bits)) - 1);
	}

//Sort to give an array of  rank -> (docid, score)
std::sort(docs1, docs1 + pregen.doc_count, compare_rsv_greater);

printf("    rank    docid                  rsv title\n");
for (long long i = 0; i < pregen.doc_count; i++)
	printf("%8lld %8llu %20llu %s\n", i, docs1[i].first, docs1[i].second, docnames[docs1[i].first]);

printf("\n");

delete [] docs1;
}

int main(int argc, char **argv)
{
char *empty_string = "";
FILE *doclist;
std::vector<char*> docnames;
char line[4096];

if (argc < 3)
	{
	printf("Usage: %s <doclistfile> <pregenfile> ...\n", argv[0]);
	return -1;
	}

doclist = fopen(argv[1], "rb");

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

fprintf(stderr, "Printing pregens...\n");

for (int i = 2; i < argc; i++)
	{
	ANT_pregen pregen;

	printf("Pregen on field %s of type %s:\n", pregen.field_name, pregen_type_to_str(pregen.type));

	pregen.read(argv[i]);

	if (pregen.doc_count != docnames.size())
		{
		printf("Pregen doc count %llu doesn't match doclist count %llu\n", pregen.doc_count, docnames.size());
		}
	else
		print_pregen(docnames, pregen, 64);
	}

//Leak the whole doclist
return 0;
}
