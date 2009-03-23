/*
	TERM_FREQUENCIES.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/disk.h"

#define MAX 10000000
long pos[10];

int main(int argc, char *argv[])
{
ANT_disk disk;
char *file, **lines, **current;
long long number_of_terms;
long power, times, df;

if (argc != 2)
	exit(printf("Usage:%s <infile>\n"));
file = disk.read_entire_file(argv[1]);
lines = disk.buffer_to_list(file, &number_of_terms);

for (current = lines; *current != NULL; current++)
	{
	sscanf(*current, "%*s %*lld %d", &df);
	for (times = power = 1; power < MAX; times = times + 1, power *= 10)
		if (df < power)
			{
			pos[times]++;
			break;
			}
	}
for (times = power = 1; power < MAX; times = times + 1, power *= 10)
	printf("%d %d\n", power, pos[times]);
}

