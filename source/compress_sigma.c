/*
	COMPRESS_SIGMA.C
	----------------
	This code is the original Sigma code ported to ANT.  For details see:
	A. Trotman, V. Subramanya, Sigma encoded inverted files, CIKM 2007, pp 983-986

	This code has been released BSD by Andrew Trotman and Vikram Subramanya.  Here's
	the Email Exchange:


	> From: "Vikram Subramanya" <vikram.s3@cs.ucsd.edu>
	> To: "Andrew Trotman" <andrew@cs.otago.ac.nz>
	> Cc: "andrew Trotman" <andrew.trotman@otago.ac.nz>
	> Sent: Wednesday, April 21, 2010 12:57 PM
	> Subject: Re: Compression Code
	>
	>
	>> Oh, absolutely. I would be delighted to see that the code we wrote will see
	>> sunshine! Thanks for asking.
	>> 
	>> Vikram
	>> 
	>> 
	>> On Tue, Apr 20, 2010 at 5:53 PM, Andrew Trotman <andrew@cs.otago.ac.nz>wrote:
	>> 
	>>> Hi Vikram,
	>>>
	>>> I hope all is well.
	>>>
	>>> I've been working on a new search engine and I want to release my changes
	>>> to our code under the BSD licence.  My university is asking that I get your
	>>> agreement for this as you contributed to the code base.  Are you happy for
	>>> our code to be released under the BSD license?
	>>>
	>>> Thanks
	>>> Andrew.
	>>>
	>>>
	>> 
	>> 
	>> -- 
	>> Vikram Subramanya
	>> 
	>> Grad Student | UCSD CSE
	>>
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "pragma.h"
#include "compress_sigma.h"
#include "compress_carryover12.h"
#include "compress_carryover12_internals.h"
#include "fundamental_types.h"
#include "maths.h"
#include "compress_sigma_frequency.h"

#pragma ANT_PRAGMA_CONST_CONDITIONAL

/*
	ANT_COMPRESS_SIGMA::MAP_CMP()
	-----------------------------
*/
int ANT_compress_sigma::map_cmp(const void *a, const void *b)
{
ANT_compress_sigma_frequency *first, *second;
first = (ANT_compress_sigma_frequency *)a;
second = (ANT_compress_sigma_frequency *)b;

if (first->gap == second->gap)
	return first->index_pos > second->index_pos ? 1 : -1;
else
	return first->gap > second->gap ? 1 : -1;
}

/*
	ANT_COMPRESS_SIGMA::MAP_FREQ_CMP()
	----------------------------------
*/
int ANT_compress_sigma::map_freq_cmp(const void *a, const void *b)
{
ANT_compress_sigma_frequency *first, *second;
first = (ANT_compress_sigma_frequency *)a;
second = (ANT_compress_sigma_frequency *)b;

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
int ANT_compress_sigma::long_cmp(const void *a, const void *b)
{
ANT_compressable_integer one, two;

one = *(ANT_compressable_integer *)a;
two = *(ANT_compressable_integer *)b;

return a > b ? 1 : (a == b ? 0 : -1);
}

/*
	ANT_COMPRESS_SIGMA::REORDER()
	-----------------------------
*/
ANT_compress_sigma_frequency *ANT_compress_sigma::reorder(ANT_compress_sigma_frequency *map, ANT_compress_sigma_frequency *end, long uniques, unsigned long threshold, ANT_compressable_integer *uniques_over_threshold)
{
ANT_compress_sigma_frequency *current, *preorder, *gap;
ANT_compressable_integer last, pow, from, to;

gap = preorder = new ANT_compress_sigma_frequency[uniques];
*uniques_over_threshold = 0;
last = sizeof(last) == 4 ? LONG_MAX : LLONG_MAX;		// the compiler should work this out
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
	from = ANT_pow2_zero(pow);
	to = ANT_pow2_zero(pow + 1);
	if (to > *uniques_over_threshold)
		to = *uniques_over_threshold;
	qsort(preorder + from, to - from, sizeof(*preorder), long_cmp);
	}

return preorder;
}

/*
	ANT_COMPRESS_SIGMA::COMPRESS()
	------------------------------
*/
long long ANT_compress_sigma::compress(unsigned char *target, long long destination_length, ANT_compressable_integer *source, long long size)
{
ANT_compressable_integer *destination, *gap, *list;
ANT_compress_sigma_frequency *preorder;
long index = 0;
long raw_size, final_size;
ANT_compressable_integer uniques_over_threshold;
ANT_compress_sigma_frequency *map, *end, *from, *equal_freq, *current;
ANT_compressable_integer *integer, last, uniques, freq, temp, diff;
long pow, p_from, to;

/*
	Allocate space needed to compute the frequencies
*/
map = new ANT_compress_sigma_frequency[(size_t)size];
end = map + size;

/*
	copy the source array
*/
integer = source;
for (current = map; current < end; current++)
	{
	current->gap = *integer;
	current->index_pos = integer - source;
	current->freq = 0;
	integer++;
	}

/*
	sort it.
*/
qsort(map, (size_t)size, sizeof(*map), map_cmp);

/*
	Compute the frequences of each unique value in the source array
*/
last = sizeof(last) == 4 ? LONG_MAX : LLONG_MAX;		// the compiler should work this out
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

/*
	Sort the unique values by frequency
*/
qsort(map, (size_t)size, sizeof(*map), map_freq_cmp);

/*
	Now sort into increasing order within whole powers of 2
*/
preorder = reorder(map, end, uniques, threshold, &uniques_over_threshold);

/*
	Now run through and do the sigma encoding
*/
raw_size = uniques_over_threshold + 1 + size;
destination = new ANT_compressable_integer[raw_size];
gap = destination + 1;
list = destination + uniques_over_threshold + 1;

last = sizeof(last) == 4 ? LONG_MAX : LLONG_MAX;		// the compiler should work this out
for (current = map; current < end; current++)					// for each term in the list
	{
	if (current->gap != last)
		{
		for (index = 0; index < uniques; index++)				// for each number in the (frequency sorted) dictionary
			if (preorder[index].gap == current->gap)			// found it
				{
				if (preorder[index].freq <= threshold)			// frequency is less than threshold so use the raw value (+base)
					index = current->gap + uniques_over_threshold;
				break;											// else use the dictionary position
				}
		last = current->gap;
		}
	list[current->index_pos] = index;							// now store the result
	}

for (index = 0; index < uniques_over_threshold; index++)
	(destination + 1)[index] = preorder[index].gap;

*destination = uniques_over_threshold;

/*
	Difference encode each set of numbers in the range [(2^n), (2^(n+1)-1)]
*/
p_from = to = 1;
for (pow = 0; to < uniques_over_threshold; pow++)		// for each power of 2
	{
	p_from = ANT_pow2_zero(pow);
	to = ANT_pow2_zero(pow + 1);
	if (to > uniques_over_threshold)
		to = uniques_over_threshold;
	last = preorder[p_from].gap;
	(destination + 1)[p_from] = last;
	for (diff = p_from + 1; diff < to; diff++)			// difference encode
		{
		temp = (destination + 1)[diff];
		(destination + 1)[diff] -= last + 1;
		last = temp;
		}
	}

#ifdef NEVER
	{
	printf("\nSIGMA--->\n");
	for (long pos = 0; pos < (raw_size < 10 ? raw_size : 10); pos++)
		printf("%d:%d\n", pos, destination[pos]);
	printf("<---SIGMA\n");
	}
#endif

/*
	Finally, compress using something that is good at compressing small numbers
*/
final_size = carryover12.compress(target, destination_length, destination, raw_size);

delete [] map;
delete [] destination;
delete [] preorder;

return final_size;		// the space sigma uses is the space that (in this case) carryover-12 takes
}

/*
	ANT_COMPRESS_SIGMA::DECOMPRESS()
	--------------------------------
*/
void ANT_compress_sigma::decompress(ANT_compressable_integer *uncompressed, unsigned char *source, long long n)
{
uint32_t *__wpos = (uint32_t *)source;
long pow;
long __wbits = TRANS_TABLE_STARTER;
long __wremaining = -1;
unsigned __wval = 0;
ANT_compressable_integer *into, *end, uniques, from, to, diff, last, got;

CARRY_BLOCK_DECODE_START;
/*
	read the length of the dictionary
*/
CARRY_DECODE(uniques);

if (uniques > dictionary_length)
	{
	delete [] dictionary;
	dictionary = new ANT_compressable_integer[dictionary_length = uniques];
	}

/*
	Decode the dictionary
*/
from = to = 0;
last = 0;
pow = 0;
into = dictionary;
while (to < uniques)
	{
	from = ANT_pow2_zero(pow++);
	to = ANT_pow2_zero(pow);
	if (to > uniques)
		to = uniques;

	CARRY_DECODE(*into++ = last);

	for (diff = from + 1; diff < to; diff++)
		{
		CARRY_DECODE(got);
		last = *into++ = got + last + 1;
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
	if (got >= uniques)
		*into++ = got - uniques;
	else
		*into++ = dictionary[got];
	}
while (into < end);
}

