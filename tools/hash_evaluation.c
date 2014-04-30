#include <stdio.h>
#include <stdlib.h>
#include <asm/cachectl.h>
#include "hash_table.h"
#include "string_pair.h"
#include "disk.h"
#include "stats.h"
#include "memory.h"

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
char **terms = ANT_disk::buffer_to_list(ANT_disk::read_entire_file(argv[1]), &number_terms);
long long total_hash_time[3] = {0,0,0};
long long now;
ANT_stats *clock = new ANT_stats(new ANT_memory);
ANT_string_pair *s = NULL;

#ifndef HASHER
	exit(printf("No valid hasher defined!\n"));
#elif HASHER==RANDOM
	printf("Random hasher\n");
#elif HASHER==RANDOM_STEP
	printf("Random (step) hasher\n");
#elif HASHER==HEADER
	printf("Header hasher\n");
#else
	exit(printf("Unknown hasher\n"));
#endif

for (int repeats = 0; repeats < 200; repeats++)
	for (int term = 0; term < number_terms; term++)
		{
		delete s;
		s = new ANT_string_pair(terms[term]);

		cacheflush(NULL, 0, BCACHE);
		now = clock->start_timer();
		ANT_hash_8(s);
		total_hash_time[0] += clock->stop_timer(now);

		now = clock->start_timer();
		ANT_hash_24(s);
		total_hash_time[1] += clock->stop_timer(now);

		now = clock->start_timer();
		ANT_hash_32(s);
		total_hash_time[2] += clock->stop_timer(now);
		}

clock->print_time("Hashing  8-bit : ", total_hash_time[0], "");
clock->print_time("Hashing 24-bit : ", total_hash_time[1], "");
clock->print_time("Hashing 32-bit : ", total_hash_time[2], "");
printf("Terms: %lld\n", number_terms);
printf("200 repeats\n");


delete s;

return EXIT_SUCCESS;
}
