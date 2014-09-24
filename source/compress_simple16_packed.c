/*
	COMPRESS_SIMPLE16_PACKED.C
	------------------
	Simple-16-Packed, an adaptation of Simple-16.

	Author: Blake Burgess
	License: BSD (see compress_simple16.c, compress_simple9_packed.c, compress_simple9.c and compress_sigma.c)
*/
#include <stdio.h>
#include "compress_simple16_packed.h"
#include "maths.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_compress_simple16_packed::_table[]
	---------------------------------------
	Number of bits required to compress each integer -- used to select the appropriate mask
*/
long ANT_compress_simple16_packed::simple16_packed_mask_table[] =
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
	ANT_compress_simple16_packed::simple16_shift_table[]
	---------------------------------------------
	Number of bits to shift across when packing -- is sum of prior packed ints (see above)
*/
long ANT_compress_simple16_packed::simple16_packed_shift_table[] =
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
	ANT_compress_simple16_packed::ints_packed_table[]
	--------------------------------------------
	Number of integers packed into a word, given its mask type
*/
long ANT_compress_simple16_packed::ints_packed_table[] = {28, 21, 21, 21, 14, 9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 1};

/*
	ANT_compress_simple16_packed::bits_to_use[]
	-----------------------------------
	This is the number of bits that simple-16 will be used to store an integer of the given the number of bits in length
*/
long ANT_compress_simple16_packed::bits_to_use[] = 
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
	ANT_COMPRESS_SIMPLE16_PACKED::CAN_PACK()
	---------------------------------
	Try to pack integers according to the mask type
*/
long ANT_compress_simple16_packed::can_pack(ANT_compressable_integer *from, int mask_type, int pack_limit)
{
int offset = 0;
pack_limit = (pack_limit < ints_packed_table[mask_type]) ? pack_limit: ints_packed_table[mask_type];
while (offset < pack_limit)
	{
	if (*from++ >= 1 << simple16_packed_mask_table[offset + 28 * mask_type])
		return 0;
	offset++;
	}
return 1;
}

/*
	ANT_COMPRESS_SIMPLE16_PACKED:PACK()
	----------------------------
	Pack integers into word.
	TODO unroll this back into the compress() function.
*/
void ANT_compress_simple16_packed::pack(ANT_compressable_integer *source, uint32_t *dest, uint32_t mask_type, uint32_t num_to_pack)
{
	int bits_to_shift;
	int offset = 0;
	int mask_type_offset = 28 * mask_type;
	*dest = 0;
	while (offset < num_to_pack)
		{
		bits_to_shift = simple16_packed_shift_table[mask_type_offset + offset];
		*dest |= *(source + offset) << bits_to_shift;
		offset++;
		}
	*dest = *dest << 4 | mask_type;
}

/*
	ANT_COMPRESS_SIMPLE16_PACKED::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple16_packed::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers) {
long long words_in_compressed_string, pos;
long num_to_pack;
uint32_t mask_type;
uint32_t *into = (uint32_t *)destination;

// possibly allocate more memory 
if (source_integers > blocks_length)
	{
	delete blocks_needed;
	delete masks;
	blocks_needed = new long [source_integers];
	masks = new unsigned char [source_integers];
	blocks_length = source_integers;
	}

words_in_compressed_string = 0;

// optimization fails if we only have one integer (due to out-of-bounds access)
if (source_integers == 1)
	{
	pack(source, into, 15, 1); // choose largest bits-per-integer mask, single integer
	words_in_compressed_string++;
	return words_in_compressed_string * sizeof(*into);
	}

/* initialise */
pos = 0;
while (pos < source_integers)
	{
	blocks_needed[pos] = -1; // INFINITY value
	masks[pos] = 0;
	pos++;
	}

/* init last value to pack-by-self */
pos = source_integers - 1;
blocks_needed[pos] = 0;
masks[pos] = 15;

/* optimise from second-last value */
pos = source_integers - 2;

/* optimise packing masks to use */
while (pos >= 0)
{
	for (mask_type = 0; mask_type < 16; mask_type++)
	{
		num_to_pack = (pos + ints_packed_table[mask_type] > source_integers) ? source_integers - pos : ints_packed_table[mask_type];
		if (can_pack(source + pos, mask_type, num_to_pack) && pos + ints_packed_table[mask_type] <= source_integers)
			if (pos + num_to_pack >= source_integers)
			{
				blocks_needed[pos] = 1;
				masks[pos] = mask_type;
			}
			else if (blocks_needed[pos] == -1 || blocks_needed[pos] > blocks_needed[pos+num_to_pack] + 1)
			{
				blocks_needed[pos] = blocks_needed[pos+num_to_pack] + 1;
				masks[pos] = mask_type;
			}
		/* fail case: can't pack current block (i.e. x > 2^28) */
		else if (mask_type == 16)
			return 0;
	}
	pos--;
}

/* now actually pack */
pos = 0;
while (pos < source_integers)
{
	mask_type = masks[pos];
	num_to_pack = (pos + ints_packed_table[mask_type] > source_integers) ? source_integers - pos : ints_packed_table[mask_type];
	pack(source + pos, into, mask_type, num_to_pack);
	pos += num_to_pack;
	into++;
	words_in_compressed_string++;
}
return words_in_compressed_string * sizeof(*into);
}

/*
	ANT_COMPRESS_SIMPLE16_PACKED::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple16_packed::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
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
