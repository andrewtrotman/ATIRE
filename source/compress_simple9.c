/*
	COMPRESS_SIMPLE9.C
	------------------
	Anh and Moffat's Simple-9 Compression scheme from:
	V. Anh, A. Moffat (2005), Inverted Index Compression Using Word-Alligned Binary Codes, Information Retrieval, 8(1):151-166

	This code was originally written by Vikram Subramanya while working on:
	A. Trotman, V. Subramanya (2007), Sigma encoded inverted files, Proceedings of CIKM 2007, pp 983-986

	Converted for use in ANT by Andrew Trotman (2009)
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
	ANT_compress_simple9::simple9_table
	-----------------------------------
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

long bits_to_use [] =
{
0,		// 0
1,		// 1
2,		// 2
3,		// 3
4,		// 4
5,		// 5
7,		// 6
7,		// 7
9,		// 8
9,		// 9
14,	// 10
14,	// 11
14,	// 12
14,	// 13
14,	// 14
28,	// 15
28,	// 16
28,	// 17
28,	// 18
28,	// 19
28,	// 20
28,	// 21
28,	// 22
28,	// 23
28,	// 24
28,	// 25
28,	// 26
28,	// 27
28,	// 28
28		// 29
} ;

long table_row[] =
{
0,		// 1 integer
1,		// 2 integers
2,		// 3
3,		// 4
4,		// 5
5,		// 6
5,		// 7
6,		// 8
6,		// 9
7,		// 10
7,		// 11
7,		// 12
7,		// 13
7,		// 14
8,		// 15
8,		// 16
8,		// 17
8,		// 18
8,		// 19
8,		// 20
8,		// 21
8,		// 22
8,		// 23
8,		// 24
8,		// 25
8,		// 26
8,		// 27
8,		// 28
8		// 29
} ;

/*
	DOESHIGHESTFIT()
	----------------
	This function checks whether the highest no. in the 
	range d[pos] to d[pos + noOfDigits] can fit in "noOfBits" bits
*/
long ANT_compress_simple9::DoesHighestFit(ANT_compressable_integer d[], long pos, long noOfDigits, long noOfBits, long size)
{
long i = 0, highest;

highest = d[pos];

for (i = pos + 1; i < (pos + noOfDigits) && i < size ; i++)
	if (highest < d[i])
		highest = d[i];   //stores the highest no. in "highest"

if (highest < (((unsigned long)1) << noOfBits))  //checks if "highest" fits in noOfBits
	return TRUE;
else 
	return FALSE;
}

/*
	ANT_COMPRESS_SIMPLE9::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple9::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long needed_for_this_integer, needed;
unsigned long *into = (unsigned long *)destination;
long m = 0, j = 0, pos = 0, start = 0, temp = 0, k = 0;
long noOfDigits = 0, noOfBits = 0, row = 0, i = 0;

for (j = 0; pos < source_integers; j++)  //outer loop: loops thru' all the elements in source[]
	{
	needed = 0;
	for (i = 0; i < 28 && pos + i < source_integers; i++)
		{
		needed_for_this_integer = bits_to_use[ANT_ceiling_log2(source[pos + i])];
		if (needed_for_this_integer > 28 || needed_for_this_integer < 1)
			return 0;					// we fail because there is an integer greater then 2^28 (or 0) and so we cannot pack it
		if (needed_for_this_integer > needed)
			needed = needed_for_this_integer;
		if (needed * i >= 28)				// then we'll overflow so break out
			break;
		}
	start = pos;
	row = table_row[i - 1];
	noOfDigits = simple9_table[row].numbers;
	noOfBits = simple9_table[row].bits;
	pos = pos + noOfDigits;  //updates the position

	*into = row << 28;   //puts the row no. to the first 4 bits.
	m = 0;
	for (k = start; k < pos && k < source_integers; k++)  //puts the next noOfDigits of source[] into 1 word n[j]
		*into |= ((unsigned long)source[k]) << (m++ * noOfBits);  //left shift the bits to the correct position in n[j]
	into++;
	}
return j * 4;  //stores the length of n[]
}

/*
	DECOMPRESSSIM9()
	----------------
	this function decompresses the n[] array to get back differences in p[]	
*/
//inline void DecompressSim9(long *n, long compressedLength, long *into)
/*
	ANT_COMPRESS_SIMPLE9::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple9::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
long long decompressed_integers = 0;
long long numbers;
long mask, bits;
uint32_t *compressed_sequence = (uint32_t *)source;
uint32_t value, row;

while (1)		// we break out of this loop in the case of overflow of the destination buffer
	{
	value = *compressed_sequence++;
	row = value >> 28;						// row number stored in high 4 bits
	value &= 0x0fffffff;

	/*
		Load the details from the lookup table so as to
		avoid the dereference each decode.
	*/
	bits = simple9_table[row].bits;
	mask = simple9_table[row].mask;
	numbers = simple9_table[row].numbers;
	decompressed_integers += numbers;

	if (decompressed_integers < destination_integers)
		while (numbers-- > 0)		// Extract "numbers" number of integers from the word
			{
			*destination++ = value & mask;		// mask the current integer
			value >>= bits;						// shift to the next integer
			}
	else
		{
		numbers -= decompressed_integers - destination_integers;
		while (numbers-- > 0)		// Extract "numbers" number of integers from the word
			{
			*destination++ = value & mask;		// mask the current integer
			value >>= bits;						// shift to the next integer
			}
		break;
		}
	}
}

