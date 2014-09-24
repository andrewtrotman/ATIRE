/*
	COMPRESS_SIMPLE16.C
	------------------
	Simple-16, an adaptation of Simple-9.

	This code is a substantial rewrite of compress_simple9.c, merged with the ideas developed in the following papers:
	Zhang J, Long X, Suel T. (April 2008) Performance of compressed inverted list caching in search engines. Proceeedings of 17th Conference on the World Wide Web, pp 387-396 
	see http://www2008.org/papers/pdf/p387-zhangA.pdf
	also see http://www2009.org/proceedings/pdf/p401.pdf

	Author: Blake Burgess
	License: BSD (see compress_simple9.c and compress_sigma.c)
*/
#include <stdio.h>
#include "compress_simple16.h"
#include "maths.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_compress_simple16::simple16_mask_table[]
	---------------------------------------
	Number of bits required to compress each integer -- used to select the appropriate mask
*/
long ANT_compress_simple16::simple16_mask_table[] =
{
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
3, 4, 4, 4, 4, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
5, 5, 5, 5, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
6, 6, 6, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
5, 5, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
10, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
14, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}; 

/*
	ANT_compress_simple16::simple16_shift_table[]
	---------------------------------------------
	Number of bits to shift across when packing -- is sum of prior packed ints (see above)
*/
long ANT_compress_simple16::simple16_shift_table[] =
{
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
0, 2, 4, 6, 8, 10, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 28, 28, 28, 28, 28, 28,
0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 15, 17, 19, 21, 22, 23, 24, 25, 26, 27, 28, 28, 28, 28, 28, 28, 28,
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28,
0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 4, 7, 10, 13, 16, 19, 22, 25, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 3, 7, 11, 15, 19, 22, 25, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 4, 8, 12, 16, 20, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 5, 10, 15, 20, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 4, 8, 13, 18, 23, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 6, 12, 18, 23, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 5, 10, 16, 22, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 7, 14, 21, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 10, 19, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 14, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28
};

/*
	ANT_compress_simple16::ints_packed_table[]
	--------------------------------------------
	Number of integers packed into a word, given its mask type
*/
long ANT_compress_simple16::ints_packed_table[] = {28, 21, 21, 21, 14, 9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 1};

/*
	ANT_compress_simple16::bits_to_use[]
	-----------------------------------
	This is the number of bits that simple-16 will be used to store an integer of the given the number of bits in length
*/
long ANT_compress_simple16::bits_to_use[] = 
{
 1,  1,  2,  3,  4,  5,  6,  7, 
 9,  9, 10, 14, 14, 14, 14, 28, 
28, 28, 28, 28, 28, 28, 28, 28, 
28, 28, 28, 28, 28, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64
};

/*
	ANT_COMPRESS_SIMPLE16::CAN_PACK()
	---------------------------------
	Try to pack integers according to the mask type
*/
long ANT_compress_simple16::can_pack(ANT_compressable_integer *from, int mask_type, int pack_limit)
{
int offset = 0;
pack_limit = (pack_limit < ints_packed_table[mask_type]) ? pack_limit: ints_packed_table[mask_type];
while (offset < pack_limit)
	{
	if (*from++ >= 1 << simple16_mask_table[offset + 28 * mask_type])
		return 0;
	offset++;
	}
return 1;
}

/*
	ANT_COMPRESS_SIMPLE16:PACK()
	----------------------------
	Pack integers into word.
	TODO unroll this back into the compress() function.
*/
void ANT_compress_simple16::pack(ANT_compressable_integer *source, uint32_t *dest, uint32_t mask_type, uint32_t num_to_pack)
{
	int bits_to_shift;
	int offset = 0;
	int mask_type_offset = 28 * mask_type;
	*dest = 0;
	while (offset < num_to_pack)
		{
		bits_to_shift = simple16_shift_table[mask_type_offset + offset];
		*dest |= *(source + offset) << bits_to_shift;
		offset++;
		}
	*dest = *dest << 4 | mask_type;
}

/*
	ANT_COMPRESS_SIMPLE16::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple16::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long long words_in_compressed_string, pos;
long num_to_pack;
uint32_t *into;
uint32_t mask_type;
int remaining;

into = (uint32_t *)destination;
pos = 0;
for (words_in_compressed_string = 0; pos < source_integers; words_in_compressed_string++) // Loop through every word in source array
	{
	remaining = (pos + 28 < source_integers) ? 28 : source_integers - pos;
	mask_type = 0;
	while (mask_type < 16)
		{
		if (can_pack(source + pos, mask_type, remaining) && ints_packed_table[mask_type] <= remaining)
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

return words_in_compressed_string * sizeof(*into);  //stores the length of n[]
}

/*
	ANT_COMPRESS_SIMPLE16::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple16::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
uint32_t *compressed_sequence = (uint32_t *)source;
uint32_t value, mask_type;
ANT_compressable_integer *end = destination + destination_integers;

while (destination < end)
	{
	value = *compressed_sequence++;
	mask_type = value & 0xF;
	value >>= 4;

	// Unrolled loop to enable pipelining
	switch (mask_type)
		{
		case 0x0:
			*destination++ = value & 0x1;
			*destination++ = (value >> 0x1) & 0x1;
			*destination++ = (value >> 0x2) & 0x1;
			*destination++ = (value >> 0x3) & 0x1;
			*destination++ = (value >> 0x4) & 0x1;
			*destination++ = (value >> 0x5) & 0x1;
			*destination++ = (value >> 0x6) & 0x1;
			*destination++ = (value >> 0x7) & 0x1;
			*destination++ = (value >> 0x8) & 0x1;
			*destination++ = (value >> 0x9) & 0x1;
			*destination++ = (value >> 0xA) & 0x1;
			*destination++ = (value >> 0xB) & 0x1;
			*destination++ = (value >> 0xC) & 0x1;
			*destination++ = (value >> 0xD) & 0x1;
			*destination++ = (value >> 0xE) & 0x1;
			*destination++ = (value >> 0xF) & 0x1;
			*destination++ = (value >> 0x10) & 0x1;
			*destination++ = (value >> 0x11) & 0x1;
			*destination++ = (value >> 0x12) & 0x1;
			*destination++ = (value >> 0x13) & 0x1;
			*destination++ = (value >> 0x14) & 0x1;
			*destination++ = (value >> 0x15) & 0x1;
			*destination++ = (value >> 0x16) & 0x1;
			*destination++ = (value >> 0x17) & 0x1;
			*destination++ = (value >> 0x18) & 0x1;
			*destination++ = (value >> 0x19) & 0x1;
			*destination++ = (value >> 0x1A) & 0x1;
			*destination++ = (value >> 0x1B) & 0x1;
			break;
		case 0x1:
			*destination++ = value & 0x3;
			*destination++ = (value >> 0x2) & 0x3;
			*destination++ = (value >> 0x4) & 0x3;
			*destination++ = (value >> 0x6) & 0x3;
			*destination++ = (value >> 0x8) & 0x3;
			*destination++ = (value >> 0xA) & 0x3;
			*destination++ = (value >> 0xC) & 0x3;
			*destination++ = (value >> 0xE) & 0x1;
			*destination++ = (value >> 0xF) & 0x1;
			*destination++ = (value >> 0x10) & 0x1;
			*destination++ = (value >> 0x11) & 0x1;
			*destination++ = (value >> 0x12) & 0x1;
			*destination++ = (value >> 0x13) & 0x1;
			*destination++ = (value >> 0x14) & 0x1;
			*destination++ = (value >> 0x15) & 0x1;
			*destination++ = (value >> 0x16) & 0x1;
			*destination++ = (value >> 0x17) & 0x1;
			*destination++ = (value >> 0x18) & 0x1;
			*destination++ = (value >> 0x19) & 0x1;
			*destination++ = (value >> 0x1A) & 0x1;
			*destination++ = (value >> 0x1B) & 0x1;
			break;
		case 0x2:
			*destination++ = value & 0x1;
			*destination++ = (value >> 0x1) & 0x1;
			*destination++ = (value >> 0x2) & 0x1;
			*destination++ = (value >> 0x3) & 0x1;
			*destination++ = (value >> 0x4) & 0x1;
			*destination++ = (value >> 0x5) & 0x1;
			*destination++ = (value >> 0x6) & 0x1;
			*destination++ = (value >> 0x7) & 0x3;
			*destination++ = (value >> 0x9) & 0x3;
			*destination++ = (value >> 0xB) & 0x3;
			*destination++ = (value >> 0xD) & 0x3;
			*destination++ = (value >> 0xF) & 0x3;
			*destination++ = (value >> 0x11) & 0x3;
			*destination++ = (value >> 0x13) & 0x3;
			*destination++ = (value >> 0x15) & 0x1;
			*destination++ = (value >> 0x16) & 0x1;
			*destination++ = (value >> 0x17) & 0x1;
			*destination++ = (value >> 0x18) & 0x1;
			*destination++ = (value >> 0x19) & 0x1;
			*destination++ = (value >> 0x1A) & 0x1;
			*destination++ = (value >> 0x1B) & 0x1;
			break;
		case 0x3:
			*destination++ = value & 0x1;
			*destination++ = (value >> 0x1) & 0x1;
			*destination++ = (value >> 0x2) & 0x1;
			*destination++ = (value >> 0x3) & 0x1;
			*destination++ = (value >> 0x4) & 0x1;
			*destination++ = (value >> 0x5) & 0x1;
			*destination++ = (value >> 0x6) & 0x1;
			*destination++ = (value >> 0x7) & 0x1;
			*destination++ = (value >> 0x8) & 0x1;
			*destination++ = (value >> 0x9) & 0x1;
			*destination++ = (value >> 0xA) & 0x1;
			*destination++ = (value >> 0xB) & 0x1;
			*destination++ = (value >> 0xC) & 0x1;
			*destination++ = (value >> 0xD) & 0x1;
			*destination++ = (value >> 0xE) & 0x3;
			*destination++ = (value >> 0x10) & 0x3;
			*destination++ = (value >> 0x12) & 0x3;
			*destination++ = (value >> 0x14) & 0x3;
			*destination++ = (value >> 0x16) & 0x3;
			*destination++ = (value >> 0x18) & 0x3;
			*destination++ = (value >> 0x1A) & 0x3;
			break;
		case 0x4:
			*destination++ = value & 0x3;
			*destination++ = (value >> 0x2) & 0x3;
			*destination++ = (value >> 0x4) & 0x3;
			*destination++ = (value >> 0x6) & 0x3;
			*destination++ = (value >> 0x8) & 0x3;
			*destination++ = (value >> 0xA) & 0x3;
			*destination++ = (value >> 0xC) & 0x3;
			*destination++ = (value >> 0xE) & 0x3;
			*destination++ = (value >> 0x10) & 0x3;
			*destination++ = (value >> 0x12) & 0x3;
			*destination++ = (value >> 0x14) & 0x3;
			*destination++ = (value >> 0x16) & 0x3;
			*destination++ = (value >> 0x18) & 0x3;
			*destination++ = (value >> 0x1A) & 0x3;
			break;
		case 0x5:
			*destination++ = value & 0xF;
			*destination++ = (value >> 0x4) & 0x7;
			*destination++ = (value >> 0x7) & 0x7;
			*destination++ = (value >> 0xA) & 0x7;
			*destination++ = (value >> 0xD) & 0x7;
			*destination++ = (value >> 0x10) & 0x7;
			*destination++ = (value >> 0x13) & 0x7;
			*destination++ = (value >> 0x16) & 0x7;
			*destination++ = (value >> 0x19) & 0x7;
			break;
		case 0x6:
			*destination++ = value & 0x7;
			*destination++ = (value >> 0x3) & 0xF;
			*destination++ = (value >> 0x7) & 0xF;
			*destination++ = (value >> 0xB) & 0xF;
			*destination++ = (value >> 0xF) & 0xF;
			*destination++ = (value >> 0x13) & 0x7;
			*destination++ = (value >> 0x16) & 0x7;
			*destination++ = (value >> 0x19) & 0x7;
			break;
		case 0x7:
			*destination++ = value & 0xF;
			*destination++ = (value >> 0x4) & 0xF;
			*destination++ = (value >> 0x8) & 0xF;
			*destination++ = (value >> 0xC) & 0xF;
			*destination++ = (value >> 0x10) & 0xF;
			*destination++ = (value >> 0x14) & 0xF;
			*destination++ = (value >> 0x18) & 0xF;
			break;
		case 0x8:
			*destination++ = value & 0x1F;
			*destination++ = (value >> 0x5) & 0x1F;
			*destination++ = (value >> 0xA) & 0x1F;
			*destination++ = (value >> 0xF) & 0x1F;
			*destination++ = (value >> 0x14) & 0xF;
			*destination++ = (value >> 0x18) & 0xF;
			break;
		case 0x9:
			*destination++ = value & 0xF;
			*destination++ = (value >> 0x4) & 0xF;
			*destination++ = (value >> 0x8) & 0x1F;
			*destination++ = (value >> 0xD) & 0x1F;
			*destination++ = (value >> 0x12) & 0x1F;
			*destination++ = (value >> 0x17) & 0x1F;
			break;
		case 0xA:
			*destination++ = value & 0x3F;
			*destination++ = (value >> 0x6) & 0x3F;
			*destination++ = (value >> 0xC) & 0x3F;
			*destination++ = (value >> 0x12) & 0x1F;
			*destination++ = (value >> 0x17) & 0x1F;
			break;
		case 0xB:
			*destination++ = value & 0x1F;
			*destination++ = (value >> 0x5) & 0x1F;
			*destination++ = (value >> 0xA) & 0x3F;
			*destination++ = (value >> 0x10) & 0x3F;
			*destination++ = (value >> 0x16) & 0x3F;
			break;
		case 0xC:
			*destination++ = value & 0x7F;
			*destination++ = (value >> 0x7) & 0x7F;
			*destination++ = (value >> 0xE) & 0x7F;
			*destination++ = (value >> 0x15) & 0x7F;
			break;
		case 0xD:
			*destination++ = value & 0x3FF;
			*destination++ = (value >> 0xA) & 0x1FF;
			*destination++ = (value >> 0x13) & 0x1FF;
			break;
		case 0xE:
			*destination++ = value & 0x3FFF;
			*destination++ = (value >> 0xE) & 0x3FFF;
			break;
		case 0xF:
			*destination++ = value & 0xFFFFFFF;
			break;
		}
	}
}
