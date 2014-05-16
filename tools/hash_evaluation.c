#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"
#include "string_pair.h"
#include "disk.h"

#define TOTAL_REPEATS 1

/*
	MAIN()
	------
	Loads terms from a file, line by line, and times how long each of the 8,24 and 32-bit
	hashing functions takes to run on those terms.
*/
int main(int argc, char **argv)
{
if (argc != 2)
	exit(printf("Usage: %s <filename>\n", argv[0]));

long long number_terms;
char **lines = ANT_disk::buffer_to_list(ANT_disk::read_entire_file(argv[1]), &number_terms);
long long times;
char *space;
ANT_string_pair *term = NULL;
const long long hash_table_size = 1 << 24;
long long **hash_table = new long long*[3];
unsigned long hash_value;

for (int j = 0; j < 3; j++)
	{
	hash_table[j] = new long long[hash_table_size];
	for (int i = 0; i < hash_table_size; i++)
		hash_table[j][i] = 0;
	}

#ifndef HASHER
	exit(fprintf(stderr, "No valid hasher defined!\n"));
#elif HASHER == RANDOM
	fprintf(stderr, "Random\n");
#elif HASHER == RANDOM_STEP
	fprintf(stderr, "Random step\n");
#elif HASHER == HEADER
	fprintf(stderr, "Header\n");
#elif HASHER == HEADER_NUM
	fprintf(stderr, "Header + Num\n");
#elif HASHER == HEADER_EXP
	fprintf(stderr, "Header Exp\n");
#elif HASHER == SUPERFAST
	fprintf(stderr, "Superfast\n");
#elif HASHER == LOOKUP3
	fprintf(stderr, "Lookup3\n");
#else
	exit(fprintf(stderr, "Unknown hasher\n"));
#endif


for (int line = 0; line < number_terms; line++)
	{
	delete term;

	space = strchr(lines[line], ' ');
	term = new ANT_string_pair(lines[line], space - lines[line]);

	hash_value = ANT_hash_24(term);

	// count of unique terms
	hash_table[0][hash_value] += 1;

	// count of collection frequencies
	times = ANT_atoul(space + 1, strlen(space));
	hash_table[1][hash_value] += times;

	// count of document frequencies
	space = strchr(space + 1, ' ');
	times = ANT_atoul(space + 1, strlen(space));
	hash_table[2][hash_value] += times;
	}

printf("U C D\n");
for (int i = 0; i < hash_table_size; i++)
	printf("%lld %lld %lld\n", hash_table[0][i], hash_table[1][i], hash_table[2][i]);

return EXIT_SUCCESS;
}
