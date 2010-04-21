/*
	COMPRESS_SIMPLE9.C
	------------------
	Anh and Moffat's Simple-9 Compression scheme from:
	V. Anh, A. Moffat (2005), Inverted Index Compression Using Word-Alligned Binary Codes, Information Retrieval, 8(1):151-166

	This code was originally written by Vikram Subramanya while working on:
	A. Trotman, V. Subramanya (2007), Sigma encoded inverted files, Proceedings of CIKM 2007, pp 983-986

	Substantially re-written and converted for use in ANT by Andrew Trotman (2009)
	Released undeer BSD license (see compress_sigma.c)
*/
#include <stdio.h>
#include "compress_simple9.h"
#include "maths.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_compress_simple9::simple9_table[]
	-------------------------------------
	This is the simple-9 selector table (top 4 bits)
*/
ANT_compress_simple9::ANT_compress_simple9_lookup ANT_compress_simple9::simple9_table[] =
	{
	{1, 28, 0xFFFFFFF},		// integers, bits, bit-mask
	{2, 14, 0x3FFF},
	{3,  9, 0x1FF},
	{4,  7, 0x7F},
	{5,  5, 0x1F},
	{7,  4, 0xF},
	{9,  3, 0x7},
	{14, 2, 0x3},
	{28, 1, 0x1}
	};

/*
	ANT_compress_simple9::bits_to_use[]
	-----------------------------------
	This is the number of bits that simple-9 will be used to store an integer of the given the number of bits in length
*/
long ANT_compress_simple9::bits_to_use[] = 
{
 1,  1,  2,  3,  4,  5,  7,  7, 
 9,  9, 14, 14, 14, 14, 14, 28, 
28, 28, 28, 28, 28, 28, 28, 28, 
28, 28, 28, 28, 28, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64
};

/*
	ANT_compress_simple9::table_row[]
	---------------------------------
	This is the row of the table to use given the number of integers we can pack into the word
*/
long ANT_compress_simple9::table_row[] = 
{
0, 1, 2, 3, 4, 4, 5, 5, 
6, 6, 6, 6, 6, 7, 7, 7, 
7, 7, 7, 7, 7, 7, 7, 7, 
7, 7, 7, 8, 8
};

/*
	ANT_COMPRESS_SIMPLE9::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple9::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
ANT_compressable_integer *from;
long long words_in_compressed_string, pos;
long row, bits_per_integer, needed_for_this_integer, needed, term;
uint32_t *into, *end;

into = (uint32_t *)destination;
end = (uint32_t *)(destination + destination_length);
from = source;
pos = 0;
for (words_in_compressed_string = 0; pos < source_integers; words_in_compressed_string++)  //outer loop: loops thru' all the elements in source[]
	{
	needed = 0;
	for (term = 0; term < 28 && pos + term < source_integers; term++)
		{
//printf("(D:%ld B:%ld S:%ld)\n", (long)source[pos + term], (long)ANT_ceiling_log2(source[pos + term]), bits_to_use[ANT_ceiling_log2(source[pos + term])]);
		needed_for_this_integer = bits_to_use[ANT_ceiling_log2(source[pos + term])];
		if (needed_for_this_integer > 28 || needed_for_this_integer < 1)
			return 0;					// we fail because there is an integer greater then 2^28 (or 0) and so we cannot pack it
		if (needed_for_this_integer > needed)
			needed = needed_for_this_integer;
		if (needed * (term + 1) > 28)				// then we'll overflow so break out
			break;
		}
	row = table_row[term - 1];
	pos += simple9_table[row].numbers;
	bits_per_integer = simple9_table[row].bits;

//if (simple9_table[row].numbers != term)
//	printf("MM ");
//printf("(N:%ld T:%ld Bpi:%ld n:%ld)\n", (long)needed, (long)term, (long)bits_per_integer, (long)simple9_table[row].numbers);

	*into = 0;
	for (term = 0; from < source + pos; term++)
		*into |= (*from++ << (term * bits_per_integer));  //left shift the bits to the correct position in n[j]
	*into = (*into << 4) | row;		// put the selector in the bottom 4 bits

	into++;
	if (into >= end)
		return 0;
	}
return words_in_compressed_string * sizeof(*into);  //stores the length of n[]
}

/*
	ANT_COMPRESS_SIMPLE9::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple9::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
long mask, bits;
uint32_t *compressed_sequence = (uint32_t *)source;
uint32_t value, row;
ANT_compressable_integer *end = destination + destination_integers;

while (destination < end)
	{
	value = *compressed_sequence++;
	row = value & 0x0F;
	value >>= 4;

	/*
		Load the details from the lookup table so as to
		avoid the dereference each decode.
	*/
	bits = simple9_table[row].bits;
	mask = simple9_table[row].mask;

	switch (mask)			// unwind the loop
		{
		case 0x01:			// 28 integers
			*destination++ = value & mask;		// mask the current integer
			value >>= bits;						// shift to the next integer
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
		case 0x03:		// 14 integers
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
		case 0x07:		// 9 integers
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
		case 0x0F:		// 7 integers
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
		case 0x01F:		// 5 integers
			*destination++ = value & mask;
			value >>= bits;
		case 0x7F:		// 4 integers
			*destination++ = value & mask;
			value >>= bits;
		case 0x1FF:		// 3 integers
			*destination++ = value & mask;
			value >>= bits;
		case 0x3FFF:	// 2 integers
			*destination++ = value & mask;
			value >>= bits;
		case 0xFFFFFFF:	// 1 integer
			*destination++ = value;
		}
	}
}

