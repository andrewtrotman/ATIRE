#include <stdio.h>
#include <stdlib.h>
#include "memory_index.h"
#include "disk.h"
#include "hash_table.h"

volatile long long h;
volatile ANT_memory_index_hash_node *n;

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

ANT_memory_index *index = new ANT_memory_index(NULL);

long long number_terms;
long long i;
char **lines = ANT_disk::buffer_to_list(ANT_disk::read_entire_file(argv[1]), &number_terms);
ANT_string_pair **pairs = new ANT_string_pair*[number_terms];

for (i = 0; i < number_terms; i++)
	pairs[i] = new ANT_string_pair(lines[i]);

uint64_t end, start;

start = rdtsc();

for (i = 0; i < number_terms; i++)
	h = ANT_hash_24(pairs[i]);
	//n = index->add_term(pairs[i], 1, 1);

end = rdtsc();

printf("%lld\n", end - start);
}
