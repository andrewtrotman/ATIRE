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
char **lines = ANT_disk::buffer_to_list(ANT_disk::read_entire_file(argv[1]), &number_terms);
long long *times = new long long[number_terms];
unsigned long long total_hash_time[3] = {0,0,0};
long long now;
ANT_stats *clock = new ANT_stats(new ANT_memory);
ANT_string_pair **terms = NULL;

long long hash_table[1 << 24];

#ifndef HASHER
	exit(printf("No valid hasher defined!\n"));
#elif HASHER == RANDOM
	fprintf(stderr, "Random\n");
#elif HASHER == RANDOM_STEP
	fprintf(stderr, "Random step\n");
#elif HASHER == HEADER
	fprintf(stderr, "Header\n");
#elif HASHER == SUPERFAST
	fprintf(stderr, "Superfast\n");
#elif HASHER == LOOKUP3
	fprintf(stderr, "Lookup3\n");
#else
	exit(printf("Unknown hasher\n"));
#endif

char *space;
long long total_terms = 0;

ANT_string_pair *term = NULL;

for (int line = 0; line < number_terms; line++)
	{
	space = strchr(lines[line], ' ');
	times[line] = ANT_atoul(space + 1, strlen(lines[line]) - (space - lines[line]));
	delete term;
	term = new ANT_string_pair(lines[line], space - lines[line]);
	term->text_render();
	hash_table[ANT_hash_24(term)] += times[line];
	printf(" %lld %lld %lld\n", ANT_hash_24(term), 1<<24, times[line]);
	//total_terms += times[line];
	}

for (int i = 0; i < (1 << 24); i++)
	printf("%lld\n", hash_table[i]);

return EXIT_SUCCESS;

long long current_term = 0;
long long this_term;
total_terms = number_terms;
terms = new ANT_string_pair*[total_terms];

fprintf(stderr, "Creating all the terms\n");
for (int line = 0; line < number_terms; line++)
	{
	space = strchr(lines[line], ' ');
	this_term = current_term;
	new ANT_string_pair(lines[line], space - lines[line]);
//	for (int time = 1; time < times[line]; time++)
//		terms[current_term++] = terms[this_term];
	}

fprintf(stderr, "Shuffling the terms\n");
// shuffle the terms
for (unsigned long i = 0; i < total_terms - 1; i++)
	{
	unsigned long j = i + rand() / (RAND_MAX / (total_terms - i) + 1);
	ANT_string_pair *t = terms[j];
	terms[j] = terms[i];
	terms[i] = t;
	}

fprintf(stderr, "8-bit hashing\n");
for (int term = 0; term < total_terms; term++)
	{
	now = clock->start_timer();
	ANT_hash_8(terms[term]);
	total_hash_time[0] += clock->stop_timer(now);
	}
fprintf(stderr, "24-bit hashing\n");
for (int term = 0; term < total_terms; term++)
	{
	now = clock->start_timer();
	ANT_hash_24(terms[term]);
	total_hash_time[1] += clock->stop_timer(now);
	}
fprintf(stderr, "32-bit hashing\n");
for (int term = 0; term < total_terms; term++)
	{
	now = clock->start_timer();
	ANT_hash_32(terms[term]);
	total_hash_time[2] += clock->stop_timer(now);
	}

printf("%llu %llu %llu ", total_hash_time[0], total_hash_time[1], total_hash_time[2]);

return EXIT_SUCCESS;
}
