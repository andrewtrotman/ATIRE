/*
	COMPRESS_SIGMA.C
	----------------
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "compress_sigma.h"
#include "compress_carryover12.h"
#include "compress_carryover12_internals.h"

typedef struct
{
	ANT_compressable_integer gap;
	size_t index_pos;
	long long freq;
} FreqOrdinal;

static unsigned long powers_of_two[] = { 
         0,          2,        4,         8,        16,        32,         64,        128,
       256,        512,     1024,      2048,      4096,      8192,      16384,      32768,
     65536,     131072,   262144,    524288,   1048576,   2097152,    4194304,    8388608,
  16777216,   33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648};

/*
	MAP_CMP()
	---------
*/
static int map_cmp(const void *a, const void *b)
{
FreqOrdinal *first, *second;
first = (FreqOrdinal *)a;
second = (FreqOrdinal *)b;

if (first->gap == second->gap)
	return first->index_pos > second->index_pos ? 1 : -1;
else
	return first->gap > second->gap ? 1 : -1;
}

/*
	MAP_FREQ_CMP()
	--------------
*/
static int map_freq_cmp(const void *a, const void *b)
{
FreqOrdinal *first, *second;
first = (FreqOrdinal *)a;
second = (FreqOrdinal *)b;

if (second->freq != first->freq)
	return second->freq > first->freq ? 1 : -1;

if (first->gap != second->gap)
	return first->gap > second->gap ? 1 : -1;

return first->index_pos > second->index_pos ? 1 : -1;
}

/*
	ASPT_LONG_CMP()
	---------------
*/
static int aspt_long_cmp(const void *a, const void *b)
{
long *first, *second;
first = (long *)a;
second = (long *)b;

return *first - *second;
}

/*
	REORDER()
	---------
*/
static FreqOrdinal *reorder(FreqOrdinal *map, FreqOrdinal *end, long uniques, long threshold, long *uniques_over_threshold)
{
FreqOrdinal *current;
FreqOrdinal *preorder, *gap;
long last, pow, from, to;

gap = preorder = new FreqOrdinal[uniques];
*uniques_over_threshold = last = 0;
for (current = map; current < end; current++)
	{
	if (current->gap != last)
		{
		if (current->freq > threshold)
			(*uniques_over_threshold)++;
		last = current->gap;
		gap->gap = current->gap;
		gap->freq = current->freq;
		gap->index_pos = 0;
		gap++;
		}
	}

from = to = 0;
for (pow = 0; to < *uniques_over_threshold; pow++)
	{
	from = powers_of_two[pow];
	to = powers_of_two[pow + 1];
	if (to > *uniques_over_threshold)
		to = *uniques_over_threshold;
	qsort(preorder + from, to - from, sizeof(*preorder), aspt_long_cmp);
	}
/*
printf("\nInorder->\n");
for (long pos = 0; pos < uniques; pos++)
	printf("%d:%d\n", pos, preorder[pos]);
printf("<-Inorder\n");
*/

return preorder;
}


/*
	ANT_COMPRESS_SIGMA::COMPRESS()
	------------------------------
*/
long long ANT_compress_sigma::compress(unsigned char *target, long long destination_length, ANT_compressable_integer *a, long long size)
{
ANT_compress_carryover12 carryover12;
long threshold = 1;									// FIX THIS
FreqOrdinal *map, *current, *end, *from, *equal_freq;
ANT_compressable_integer *source;
ANT_compressable_integer *destination, *gap, *list;
FreqOrdinal *preorder;
long uniques, last, freq, index = 0;
long raw_size, final_size, uniques_over_threshold;

map = new FreqOrdinal[size];
end = map + size;

source = a;
for (current = map; current < end; current++)
	{
	current->gap = *source;
	current->index_pos = source - a;
	current->freq = 0;
	source++;
	}

qsort(map, size, sizeof(*map), map_cmp);

last = -1;
uniques = 0;
from = map;
freq = 1;
for (current = map; current < end; current++)
	{
	if (current->gap != last)
		{
		last = current->gap;
		uniques++;
		for (equal_freq = from; equal_freq < current; equal_freq++)
			equal_freq->freq = freq;
		from = current;
		freq = 0;
		}
	freq++;
	}
for (equal_freq = from; equal_freq < current; equal_freq++)
	equal_freq->freq = freq;

qsort(map, size, sizeof(*map), map_freq_cmp);		// order by frequencies
preorder = reorder(map, end, uniques, threshold, &uniques_over_threshold);			// now sort into increasing order by 2^n

raw_size = uniques_over_threshold + 1 + size;
destination = new ANT_compressable_integer[raw_size];
gap = destination + 1;										// Vikram's e
list = destination + uniques_over_threshold + 1;			// Vikram's f

last = 0;
for (current = map; current < end; current++)
	{
	if (current->gap != last)
		{
		for (index = 0; index < uniques; index++)
			if (preorder[index].gap == current->gap)
				{
				if (preorder[index].freq <= threshold)
					index = current->gap + uniques_over_threshold;
				break;
				}
		last = current->gap;
		}
	list[current->index_pos] = index;
	}

for (index = 0; index < uniques_over_threshold; index++)
	(destination + 1)[index] = preorder[index].gap;

*destination = uniques_over_threshold;
/*
printf("\nIn--->\n"); {
for (long pos = 0; pos < (raw_size < 10 ? raw_size : 10); pos++)
	printf("%d:%d\n", pos, destination[pos]);
} printf("<---In\n");
*/

long pow, p_from, to, diff;
p_from = to = 1;
for (pow = 0; to < uniques_over_threshold; pow++)
	{
	p_from = powers_of_two[pow];
	to = powers_of_two[pow + 1];
	if (to > uniques_over_threshold)
		to = uniques_over_threshold;
	last = preorder[p_from].gap;
	(destination + 1)[p_from] = last;
	for (diff = p_from + 1; diff < to; diff++)
		{
		long tmp;
		tmp = (destination + 1)[diff];
		(destination + 1)[diff] -= last + 1;
		last = tmp;
		}
	}

//*destination = uniques_over_threshold;
/*
printf("\nIn--->\n"); {
for (long pos = 0; pos < (raw_size < 10 ? raw_size : 10); pos++)
	printf("%d:%d\n", pos, destination[pos]);
} printf("<---In\n");
*/

final_size = carryover12.compress(target, destination_length, destination, raw_size);

delete [] map;
delete [] destination;
delete [] preorder;

return final_size;			// number of longs allocated
}


/*
	ANT_COMPRESS_SIGMA::DECOMPRESS()
	--------------------------------
*/
void ANT_compress_sigma::decompress(ANT_compressable_integer *uncompressed, unsigned char *source, long long n)
{
long *__wpos = (long *)source;
long last, uniques, got;
long from, to, diff;
unsigned long *pow;
int __wbits = TRANS_TABLE_STARTER;
int __wremaining = -1;
unsigned __wval = 0;
ANT_compressable_integer *temp, *dictionary, *into, *end;

temp = new ANT_compressable_integer [n * 3 + 100];			// fix this

CARRY_BLOCK_DECODE_START;
/*
	read the length of the dictionary
*/
CARRY_DECODE(uniques);

dictionary = temp;			// this is where we're gonna put the dictionary

/*
	Decode the dictionary
*/
from = to = 0;
last = 0;
for (pow = powers_of_two; to < uniques; /* nothing */)
	{
	from = *pow++;
	to = *pow;
	if (to > uniques)
		to = uniques;

	CARRY_DECODE(*dictionary++ = last);

	for (diff = from + 1; diff < to; diff++)
		{
		CARRY_DECODE(got);
		last = *dictionary++ = got + last + 1;
		}
	}

/*
	Decode the postings
*/
into = uncompressed;
end = into + n;
do
	{
	CARRY_DECODE(got);
	if (got > uniques)
		*into++ = got - uniques;
	else
		*into++ = temp[got];
	}
while (into < end);

delete [] temp;			// fix this
}
