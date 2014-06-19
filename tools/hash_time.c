#include <stdio.h>
#include <stdlib.h>
#include "memory_index.h"
#include "disk.h"
#include "btree_iterator.h"
#include "hash_table.h"

volatile long long h;
volatile ANT_memory_index_hash_node *n;

#ifdef TIME_HASH
	#define PROCESS(t) \
		start_clock = rdtsc(); \
		h = ANT_hash_24(t); \
		end_clock = rdtsc(); \
		total_clock += end_clock - start_clock;
#else
	#define PROCESS(t) \
		start_clock = rdtsc(); \
		n = index->add_term(term); \
		end_clock = rdtsc(); \
		total_clock += end_clock - start_clock;
#endif

/*
	RDTSC()
	-------
	Performs the rdtsc assembly instruction:
	ReaD Time Stamp Counter
	Which can be used to measure clock cycles before/after some operation
*/
inline uint64_t rdtsc()
{
uint32_t lo, hi;
__asm__ __volatile__ (
	"xorl %%eax, %%eax\n"
	"cpuid\n"
	"rdtsc\n"
	: "=a" (lo), "=d" (hi)
	:
	: "%ebx", "%ecx" );
return (uint64_t)hi << 32 | lo;
}

int main(int argc, char **argv)
{
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

#ifndef TIME_HASH
ANT_memory_index *index = new ANT_memory_index(NULL);
#endif

#define KB(x) (1024ULL * x)
#define MB(x) (1024ULL * KB(x))
#define GB(x) (1024ULL * MB(x))

const unsigned long long BUF_SIZE = GB(50ULL);

char *buffer = (char *)malloc(sizeof(*buffer) * (BUF_SIZE + 1));
char *end;
char *start;
FILE *fin = fopen(argv[1], "r");
ANT_string_pair *term = NULL;

uint64_t end_clock, start_clock;
uint64_t total_clock = 0;
size_t read;
long skipped;

read = fread(buffer, sizeof(*buffer), BUF_SIZE, fin);
buffer[read] = '\0';

while (!feof(fin))
	{
	skipped = 0;
	while (read > 0 && buffer[read] != '\n')
		{
		read--;
		skipped++;
		}

	buffer[read] = '\0';
	skipped--;

	start = buffer;
	while ((end = strchr(start, '\n')) != NULL)
		{
		delete term;
		term = new ANT_string_pair(start, end - start);

		PROCESS(term);

		start = end + 1;
		}
	delete term;
	term = new ANT_string_pair(start);

	PROCESS(term);

	// move the end of the previous buffer to the beginning
	memmove(buffer, buffer + read + 1, skipped);
	// fill the remainder of the buffer
	read = fread(buffer + skipped, sizeof(*buffer), BUF_SIZE - skipped, fin);
	read += skipped;
	}

while (buffer[read] != '\n')
	buffer[read--] = '\0';
while ((end = strchr(buffer, '\n')) != NULL)
	{
	delete term;
	term = new ANT_string_pair(buffer, end - buffer);

	PROCESS(term);

	buffer = end + 1;
	}

printf("%lld\n", total_clock);
}
