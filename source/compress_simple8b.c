/*
	COMPRESS_SIMPLE8B.C
	------------------
	Simple-8b, a 64-bit extension of Simple-9.

	This code is an adaptation of compress_simple16.c, merged with the ideas developed in the following paper:
	V. Anh, A. Moffat (2010), Index compression using 64-bit words, pp 136-138

	Author: Blake Burgess
	License: BSD

	Please see compress_simple16.c for more information.

	Original notice for compress_simple9.c:

	Anh and Moffat's Simple-9 Compression scheme from:
	V. Anh, A. Moffat (2005), Inverted Index Compression Using Word-Alligned Binary Codes, Information Retrieval, 8(1):151-166

	This code was originally written by Vikram Subramanya while working on:
	A. Trotman, V. Subramanya (2007), Sigma encoded inverted files, Proceedings of CIKM 2007, pp 983-986

	Substantially re-written and converted for use in ANT by Andrew Trotman (2009)
	Released undeer BSD license (see compress_sigma.c)
*/
#include <stdio.h>
#include "compress_simple8b.h"
#include "maths.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_COMPRESS_SIMPLE8B::INTS_PACKED_TABLE[]
	--------------------------------------------
	Number of integers packed into a 64-bit word, given its mask type
*/
const unsigned char ANT_compress_simple8b::ints_packed_table[] = {240, 120, 60, 30, 20, 15, 12, 10, 8, 7, 6, 5, 4, 3, 2, 1};

/*
	ANT_COMPRESS_SIMPLE8B::BITS_USED_TABLE[]
	--------------------------------------------
	Number of bits used to pack integers into the current 64-bit word, given its mask type
*/
const unsigned char ANT_compress_simple8b::bits_used_table[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 15, 20, 30, 60};

/*
	ANT_COMPRESS_SIMPLE8B::BITS_TO_USE[]
	-----------------------------------
	This is the number of bits that simple-8b has to use to store an integer of the given of number of bits in length
	(i.e., round 'bits required' up, to fit the next-smallest bit size that we support in this scheme)
*/
const long ANT_compress_simple8b::bits_to_use[] = 
{
 1,  1,  2,  3,  4,  5,  6,  7, 
 8, 10, 10, 12, 12, 15, 15, 15, 
20, 20, 20, 20, 20, 30, 30, 30, 
30, 30, 30, 30, 30, 30, 30, 60,
60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60,
60, 60, 60, 60, 60, 60, 60, 60
};

/*
	ANT_COMPRESS_SIMPLE8B::CAN_PACK()
	---------------------------------
	Try to pack integers according to the mask type
*/
long ANT_compress_simple8b::can_pack(ANT_compressable_integer *from, uint32_t mask_type, uint32_t pack_limit)
{
uint32_t count = 0;
uint32_t bits_this_block = bits_used_table[mask_type];
pack_limit = (pack_limit < ints_packed_table[mask_type]) ? pack_limit: ints_packed_table[mask_type];
while (count < pack_limit)
	{
	if (*from == 1 && mask_type < 2)
		{
		from++;
		count++;
		continue;
		}
	if (*from++ >= 1 << bits_this_block)
		return 0;
	count++;
	}
return 1;
}

/*
	ANT_COMPRESS_SIMPLE8B:PACK()
	----------------------------
	Pack a given number of integers into 64 bits.
	TODO unroll this back into the compress() function.
*/
void ANT_compress_simple8b::pack(ANT_compressable_integer *source, uint64_t *dest, uint32_t mask_type, uint32_t num_to_pack)
{
	uint32_t bits_to_shift = bits_used_table[mask_type];
	uint32_t bits_shifted = 0;
	uint32_t offset = 0;
	*dest = 0;
	if (mask_type > 1)
		while (offset < num_to_pack)
			{
			*dest |= ((uint64_t)*(source + offset)) << bits_shifted;
			bits_shifted += bits_to_shift;
			offset++;
			}
	*dest = (*dest << 4) | mask_type;
}

/*
	ANT_COMPRESS_SIMPLE8B::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple8b::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long long words_in_compressed_string, pos, remaining;
long num_to_pack;
uint32_t mask_type;
uint64_t *into, *end;

/* If we're using 32-bit integers, no point storing one or two integers in 64 bits.
   For one integer, we're actually _doubling_ the space required. (32 --> 64)
   For two integers, we break even, but we waste CPU cycles. (2 * 32 --> 64)
   Reanalyse this if we ever move (i.e., ANT_compressable_integer) to 64-bits
   ... But this scheme will break in other ways anyway.
 */

into = (uint64_t *)destination;
end = (uint64_t *)(destination + destination_length);

pos = 0;
for (words_in_compressed_string = 0; pos < source_integers; words_in_compressed_string++) // Loop through every word in the source array
	{
	remaining = (pos + 240 < source_integers) ? 240 : source_integers - pos;
	mask_type = 0;
	while (mask_type < 16)
		{
		num_to_pack = (pos + ints_packed_table[mask_type] > source_integers) ? source_integers - pos : ints_packed_table[mask_type];
		if (can_pack(source + pos, mask_type, num_to_pack) && ints_packed_table[mask_type] <= remaining)
			break;
		mask_type++;
		}

	if (mask_type == 16)
		return 0;

	num_to_pack = ints_packed_table[mask_type];

	pack(source + pos, into, mask_type, num_to_pack);
	pos += num_to_pack;
	into++;
	}
return words_in_compressed_string * sizeof(*into);  //return the length of n[]
}

/*
	ANT_COMPRESS_SIMPLE8B::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple8b::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
uint64_t *compressed_sequence = (uint64_t *)source;
uint32_t mask_type;
uint64_t value;
ANT_compressable_integer *end = destination + destination_integers;

while (destination < end)
	{
	// Load next compressed int, pull out the mask type used, shift to the values
	value = *compressed_sequence++;
	mask_type = value & 0xF;
	value >>= 4;

	// Unrolled loop to enable pipelining
	switch (mask_type)
		{
		case 0x0:
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			break;
		case 0x1:
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			*destination++ = 1;
			break;
		case 0x2:
			*destination++ = value & 0x1;
			*destination++ = (value >> 1) & 0x1;
			*destination++ = (value >> 2) & 0x1;
			*destination++ = (value >> 3) & 0x1;
			*destination++ = (value >> 4) & 0x1;
			*destination++ = (value >> 5) & 0x1;
			*destination++ = (value >> 6) & 0x1;
			*destination++ = (value >> 7) & 0x1;
			*destination++ = (value >> 8) & 0x1;
			*destination++ = (value >> 9) & 0x1;
			*destination++ = (value >> 10) & 0x1;
			*destination++ = (value >> 11) & 0x1;
			*destination++ = (value >> 12) & 0x1;
			*destination++ = (value >> 13) & 0x1;
			*destination++ = (value >> 14) & 0x1;
			*destination++ = (value >> 15) & 0x1;
			*destination++ = (value >> 16) & 0x1;
			*destination++ = (value >> 17) & 0x1;
			*destination++ = (value >> 18) & 0x1;
			*destination++ = (value >> 19) & 0x1;
			*destination++ = (value >> 20) & 0x1;
			*destination++ = (value >> 21) & 0x1;
			*destination++ = (value >> 22) & 0x1;
			*destination++ = (value >> 23) & 0x1;
			*destination++ = (value >> 24) & 0x1;
			*destination++ = (value >> 25) & 0x1;
			*destination++ = (value >> 26) & 0x1;
			*destination++ = (value >> 27) & 0x1;
			*destination++ = (value >> 28) & 0x1;
			*destination++ = (value >> 29) & 0x1;
			*destination++ = (value >> 30) & 0x1;
			*destination++ = (value >> 31) & 0x1;
			*destination++ = (value >> 32) & 0x1;
			*destination++ = (value >> 33) & 0x1;
			*destination++ = (value >> 34) & 0x1;
			*destination++ = (value >> 35) & 0x1;
			*destination++ = (value >> 36) & 0x1;
			*destination++ = (value >> 37) & 0x1;
			*destination++ = (value >> 38) & 0x1;
			*destination++ = (value >> 39) & 0x1;
			*destination++ = (value >> 40) & 0x1;
			*destination++ = (value >> 41) & 0x1;
			*destination++ = (value >> 42) & 0x1;
			*destination++ = (value >> 43) & 0x1;
			*destination++ = (value >> 44) & 0x1;
			*destination++ = (value >> 45) & 0x1;
			*destination++ = (value >> 46) & 0x1;
			*destination++ = (value >> 47) & 0x1;
			*destination++ = (value >> 48) & 0x1;
			*destination++ = (value >> 49) & 0x1;
			*destination++ = (value >> 50) & 0x1;
			*destination++ = (value >> 51) & 0x1;
			*destination++ = (value >> 52) & 0x1;
			*destination++ = (value >> 53) & 0x1;
			*destination++ = (value >> 54) & 0x1;
			*destination++ = (value >> 55) & 0x1;
			*destination++ = (value >> 56) & 0x1;
			*destination++ = (value >> 57) & 0x1;
			*destination++ = (value >> 58) & 0x1;
			*destination++ = (value >> 59) & 0x1;
			break;
		case 0x3:
			*destination++ = value & 0x3;
			*destination++ = (value >> 2) & 0x3;
			*destination++ = (value >> 4) & 0x3;
			*destination++ = (value >> 6) & 0x3;
			*destination++ = (value >> 8) & 0x3;
			*destination++ = (value >> 10) & 0x3;
			*destination++ = (value >> 12) & 0x3;
			*destination++ = (value >> 14) & 0x3;
			*destination++ = (value >> 16) & 0x3;
			*destination++ = (value >> 18) & 0x3;
			*destination++ = (value >> 20) & 0x3;
			*destination++ = (value >> 22) & 0x3;
			*destination++ = (value >> 24) & 0x3;
			*destination++ = (value >> 26) & 0x3;
			*destination++ = (value >> 28) & 0x3;
			*destination++ = (value >> 30) & 0x3;
			*destination++ = (value >> 32) & 0x3;
			*destination++ = (value >> 34) & 0x3;
			*destination++ = (value >> 36) & 0x3;
			*destination++ = (value >> 38) & 0x3;
			*destination++ = (value >> 40) & 0x3;
			*destination++ = (value >> 42) & 0x3;
			*destination++ = (value >> 44) & 0x3;
			*destination++ = (value >> 46) & 0x3;
			*destination++ = (value >> 48) & 0x3;
			*destination++ = (value >> 50) & 0x3;
			*destination++ = (value >> 52) & 0x3;
			*destination++ = (value >> 54) & 0x3;
			*destination++ = (value >> 56) & 0x3;
			*destination++ = (value >> 58) & 0x3;
			break;
		case 0x4:
			*destination++ = value & 0x7;
			*destination++ = (value >> 3) & 0x7;
			*destination++ = (value >> 6) & 0x7;
			*destination++ = (value >> 9) & 0x7;
			*destination++ = (value >> 12) & 0x7;
			*destination++ = (value >> 15) & 0x7;
			*destination++ = (value >> 18) & 0x7;
			*destination++ = (value >> 21) & 0x7;
			*destination++ = (value >> 24) & 0x7;
			*destination++ = (value >> 27) & 0x7;
			*destination++ = (value >> 30) & 0x7;
			*destination++ = (value >> 33) & 0x7;
			*destination++ = (value >> 36) & 0x7;
			*destination++ = (value >> 39) & 0x7;
			*destination++ = (value >> 42) & 0x7;
			*destination++ = (value >> 45) & 0x7;
			*destination++ = (value >> 48) & 0x7;
			*destination++ = (value >> 51) & 0x7;
			*destination++ = (value >> 54) & 0x7;
			*destination++ = (value >> 57) & 0x7;
			break;
		case 0x5:
			*destination++ = value & 0xF;
			*destination++ = (value >> 4) & 0xF;
			*destination++ = (value >> 8) & 0xF;
			*destination++ = (value >> 12) & 0xF;
			*destination++ = (value >> 16) & 0xF;
			*destination++ = (value >> 20) & 0xF;
			*destination++ = (value >> 24) & 0xF;
			*destination++ = (value >> 28) & 0xF;
			*destination++ = (value >> 32) & 0xF;
			*destination++ = (value >> 36) & 0xF;
			*destination++ = (value >> 40) & 0xF;
			*destination++ = (value >> 44) & 0xF;
			*destination++ = (value >> 48) & 0xF;
			*destination++ = (value >> 52) & 0xF;
			*destination++ = (value >> 56) & 0xF;
			break;
		case 0x6:
			*destination++ = value & 0x1F;
			*destination++ = (value >> 5) & 0x1F;
			*destination++ = (value >> 10) & 0x1F;
			*destination++ = (value >> 15) & 0x1F;
			*destination++ = (value >> 20) & 0x1F;
			*destination++ = (value >> 25) & 0x1F;
			*destination++ = (value >> 30) & 0x1F;
			*destination++ = (value >> 35) & 0x1F;
			*destination++ = (value >> 40) & 0x1F;
			*destination++ = (value >> 45) & 0x1F;
			*destination++ = (value >> 50) & 0x1F;
			*destination++ = (value >> 55) & 0x1F;
			break;
		case 0x7:
			*destination++ = value & 0x3F;
			*destination++ = (value >> 6) & 0x3F;
			*destination++ = (value >> 12) & 0x3F;
			*destination++ = (value >> 18) & 0x3F;
			*destination++ = (value >> 24) & 0x3F;
			*destination++ = (value >> 30) & 0x3F;
			*destination++ = (value >> 36) & 0x3F;
			*destination++ = (value >> 42) & 0x3F;
			*destination++ = (value >> 48) & 0x3F;
			*destination++ = (value >> 54) & 0x3F;
			break;
		case 0x8:
			*destination++ = value & 0x7F;
			*destination++ = (value >> 7) & 0x7F;
			*destination++ = (value >> 14) & 0x7F;
			*destination++ = (value >> 21) & 0x7F;
			*destination++ = (value >> 28) & 0x7F;
			*destination++ = (value >> 35) & 0x7F;
			*destination++ = (value >> 42) & 0x7F;
			*destination++ = (value >> 49) & 0x7F;
			break;
		case 0x9:
			*destination++ = value & 0xFF;
			*destination++ = (value >> 8) & 0xFF;
			*destination++ = (value >> 16) & 0xFF;
			*destination++ = (value >> 24) & 0xFF;
			*destination++ = (value >> 32) & 0xFF;
			*destination++ = (value >> 40) & 0xFF;
			*destination++ = (value >> 48) & 0xFF;
			break;
		case 0xA:
			*destination++ = value & 0x3FF;
			*destination++ = (value >> 10) & 0x3FF;
			*destination++ = (value >> 20) & 0x3FF;
			*destination++ = (value >> 30) & 0x3FF;
			*destination++ = (value >> 40) & 0x3FF;
			*destination++ = (value >> 50) & 0x3FF;
			break;
		case 0xB:
			*destination++ = value & 0xFFF;
			*destination++ = (value >> 12) & 0xFFF;
			*destination++ = (value >> 24) & 0xFFF;
			*destination++ = (value >> 36) & 0xFFF;
			*destination++ = (value >> 48) & 0xFFF;
			break;
		case 0xC:
			*destination++ = value & 0x7FFF;
			*destination++ = (value >> 15) & 0x7FFF;
			*destination++ = (value >> 30) & 0x7FFF;
			*destination++ = (value >> 45) & 0x7FFF;
			break;
		case 0xD:
			*destination++ = value & 0xFFFFF;
			*destination++ = (value >> 20) & 0xFFFFF;
			*destination++ = (value >> 40) & 0xFFFFF;
			break;
		case 0xE:
			*destination++ = value & 0x3FFFFFFF;
			*destination++ = (value >> 30) & 0x3FFFFFFF;
			break;
		case 0xF:
			*destination++ = value & 0xFFFFFFFFFFFFFFFL;
			break;
		}
	}
}

