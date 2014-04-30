#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"
#include "string_pair.h"
#include "disk.h"
#include "stats.h"
#include "memory.h"

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
long long number_occurrences = 0;
char **terms = ANT_disk::buffer_to_list(ANT_disk::read_entire_file(argv[1]), &number_terms);
unsigned long long total_hash_time[3] = {0,0,0};
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

// shuffle the array to try minimise caching effects
for (unsigned long i = 0; i < number_terms - 1; i++)
	{
	unsigned long j = i + rand() / (RAND_MAX / (number_terms - i) + 1);
	char *t = terms[j];
	terms[j] = terms[i];
	terms[i] = t;
	}

char *space = NULL;
unsigned long times;

for (int repeats = 0; repeats < TOTAL_REPEATS; repeats++)
	{
	total_hash_time[0] = total_hash_time[1] = total_hash_time[2] = 0;

	for (int term = 0; term < number_terms; term++)
		{
		delete s;
		space = strchr(terms[term], ' ');
		s = new ANT_string_pair(terms[term], space - terms[term]);
		times = ANT_atoul(space + 1, strlen(terms[term]) - s->length());

		for (int j = 0; j < times; j++)
			{
			now = clock->start_timer();
			ANT_hash_8(s);
			total_hash_time[0] += clock->stop_timer(now);
			}
		}

	for (int term = 0; term < number_terms; term++)
		{
		delete s;
		space = strchr(terms[term], ' ');
		s = new ANT_string_pair(terms[term], space - terms[term]);
		times = ANT_atoul(space + 1, strlen(terms[term]) - s->length());

		for (int j = 0; j < times; j++)
			{
			now = clock->start_timer();
			ANT_hash_24(s);
			total_hash_time[1] += clock->stop_timer(now);
			}
		}

	for (int term = 0; term < number_terms; term++)
		{
		delete s;
		space = strchr(terms[term], ' ');
		s = new ANT_string_pair(terms[term], space - terms[term]);
		times = ANT_atoul(space + 1, strlen(terms[term]) - s->length());

		for (int j = 0; j < times; j++)
			{
			now = clock->start_timer();
			ANT_hash_32(s);
			total_hash_time[2] += clock->stop_timer(now);
			}
		}

	clock->print_time("Hashing  8-bit : ", total_hash_time[0], "");
	clock->print_time("Hashing 24-bit : ", total_hash_time[1], "");
	clock->print_time("Hashing 32-bit : ", total_hash_time[2], "");
	}

printf("Terms: %lld\n", number_terms);

return EXIT_SUCCESS;
}
