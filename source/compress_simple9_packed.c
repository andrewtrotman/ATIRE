/*
	COMPRESS_SIMPLE9_PACKED.C
	------------------
	Michael Albert -- use of block packing in Java (generic algorithm)
	Rewritten in C and then substantially adapted for use in ATIRE by Blake Burgess (Sep 2014)
	(Via modifications from compress_simple{9,16}.c)
	Released under BSD license, subject to conditions in compress_simple9.c and compress_simple16.c

	Currently a splice of simple9 and simple16. Implementation could be cleaner.
*/
#include <stdio.h>
#include "compress_simple9_packed.h"
#include "maths.h"

#define FIND_FIRST_SET 1+ANT_floor_log2
#define FIND_LAST_SET ANT_ceiling_log2

/*
	ANT_COMPRESS_SIMPLE9_PACKED::SIMPLE9_PACKED_SHIFT_TABLE[]
	---------------------------------------------
	Number of bits to shift across when packing -- is sum of prior packed ints (see above)
*/
long ANT_compress_simple9_packed::simple9_packed_shift_table[] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
0, 4, 8, 12, 16, 20, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 5, 10, 15, 20, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
0, 7, 14, 21, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 9, 18, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
0, 14, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28
};

/*
	ANT_COMPRESS_SIMPLE9_PACKED::INTS_PACKED_TABLE[]
	--------------------------------------------
	Number of integers packed into a word, given its mask type
*/
long ANT_compress_simple9_packed::ints_packed_table[] = {28, 14, 9, 7, 5, 4, 3, 2, 1};

/*
	ANT_COMPRESS_SIMPLE9_PACKED::CAN_PACK_TABLE[]
	---------------------------------------------
	Bitmask map for valid masks at an offset (column) for some num_bits_needed (row).
*/
long ANT_compress_simple9_packed::can_pack_table[] = 
{
0x01ff, 0x00ff, 0x007f, 0x003f, 0x001f, 0x000f, 0x000f, 0x0007, 0x0007, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
0x01fe, 0x00fe, 0x007e, 0x003e, 0x001e, 0x000e, 0x000e, 0x0006, 0x0006, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x01fc, 0x00fc, 0x007c, 0x003c, 0x001c, 0x000c, 0x000c, 0x0004, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x01f8, 0x00f8, 0x0078, 0x0038, 0x0018, 0x0008, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x01f0, 0x00f0, 0x0070, 0x0030, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x01e0, 0x00e0, 0x0060, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x01c0, 0x00c0, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0180, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

/*
	ANT_COMPRESS_SIMPLE9_PACKED::INVALID_MASKS_FOR_OFFSET[]
	-----------------------------------
	We AND out masks for offsets where we don't know if we can fully pack for that offset
*/
long ANT_compress_simple9_packed::invalid_masks_for_offset[] = 
{
0x0000, 0x0100, 0x0180, 0x01c0, 0x01e0, 0x01f0, 0x01f0, 0x01f8, 0x01f8, 0x01fc, 0x01fc, 0x01fc, 0x01fc, 0x01fc, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01ff
};

/*
	ANT_COMPRESS_SIMPLE9_PACKED::ROW_FOR_BITS_NEEDED[]
	-----------------------------------
	Translates the 'bits_needed' to the appropriate 'row' offset for use with can_pack table.
*/
long ANT_compress_simple9_packed::row_for_bits_needed[] = 
{
0, 0, 28, 56, 84, 112, 140, 140, 168, 168, 196, 196, 196, 196, 196, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
	ANT_COMPRESS_SIMPLE9_PACKED::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple9_packed::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long long words_in_compressed_string, pos;
uint32_t mask_type, num_to_pack;
uint32_t *into, *end;
uint32_t remaining;
uint32_t offset, mask_type_offset;
uint16_t last_bitmask, bitmask;
into = (uint32_t *)destination;
end = (uint32_t *)(destination + destination_length);
/* possibly allocate more memory */
if (source_integers > blocks_length)
	{
	delete blocks_needed;
	delete masks;
	blocks_needed = new long [source_integers];
	masks = new unsigned char [source_integers];
	blocks_length = source_integers;
	}
words_in_compressed_string = 0;
/* optimization fails if we only have one integer (due to out-of-bounds access) */
if (source_integers == 1)
	{
	/* choose largest bits-per-integer mask, single integer => '8' */
	mask_type_offset = 28 * 8;
	*into = (*source << simple9_packed_shift_table[mask_type_offset]);
	*into = (*into << 4) | 8;
	words_in_compressed_string++;
	return words_in_compressed_string * sizeof(*into);
	}
/* initialize */
pos = 0;
while (pos < source_integers)
	{
	blocks_needed[pos] = -1; // INFINITY value
	masks[pos] = 255; // JUNK mask value
	pos++;
	}
/* init last value to pack-by-self */
pos = source_integers - 1;
blocks_needed[pos] = 0;
masks[pos] = 8;
/* optimize from second-last value */
pos = source_integers - 2;
/* optimize packing masks to use */
while (pos >= 0)
	{
	remaining = (pos + 28 < source_integers) ? 28 : source_integers - pos;
	last_bitmask = 0x0000;
	bitmask = 0xFFFF;
	/* constrain last_bitmask to contain only bits for masks we can pack with */
	for (offset = 0; offset < remaining && bitmask; offset++)
		{
		bitmask &= can_pack_table[row_for_bits_needed[FIND_LAST_SET(source[pos + offset])] + offset];
		last_bitmask |= (bitmask & invalid_masks_for_offset[offset + 1]);
		}
	/* no bits set => no masks work => invalid input */
	if (!last_bitmask)
		return 0;
	/* iterate through the bitmask bit-wise and try the optimization. */
	for (offset = 0; offset < 9; offset++)
		if ((1 << offset) & last_bitmask)
			{
			num_to_pack = ints_packed_table[offset];
			/* if we can pack til end w/ this mask, blocks_needed is one */
			if (pos + num_to_pack >= source_integers)
				{
				blocks_needed[pos] = 1;
				masks[pos] = offset;
				}
			/* otherwise, update blocks_needed if it is 'infinity' or if we have a shorter path */
			else if (blocks_needed[pos] == -1 || blocks_needed[pos] > blocks_needed[pos+num_to_pack] + 1)
				{
				blocks_needed[pos] = blocks_needed[pos+num_to_pack] + 1;
				masks[pos] = offset;
				}
			}
	/* fail case: can't pack current block (i.e. x > 2^28) */
	if (masks[pos] == 255)
		return 0;
	pos--;
	}
/* now actually pack */
pos = 0;
while (pos < source_integers)
	{
	mask_type = masks[pos];
	num_to_pack = (pos + ints_packed_table[mask_type] > source_integers) ? source_integers - pos : ints_packed_table[mask_type];
	/* pack the word */
	*into = 0;
	mask_type_offset = 28 * mask_type;
	for (offset = 0; offset < num_to_pack; offset++)
		*into |= (source[pos + offset] << simple9_packed_shift_table[mask_type_offset + offset]);
	*into = (*into << 4) | mask_type;
	pos += num_to_pack;
	into++;
	words_in_compressed_string++;
	if (into > end)
		return 0;
	}
return words_in_compressed_string * sizeof(*into);
}

/*
	ANT_COMPRESS_SIMPLE9_PACKED::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple9_packed::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
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
			*destination++ = (value >> 0xE) & 0x3;
			*destination++ = (value >> 0x10) & 0x3;
			*destination++ = (value >> 0x12) & 0x3;
			*destination++ = (value >> 0x14) & 0x3;
			*destination++ = (value >> 0x16) & 0x3;
			*destination++ = (value >> 0x18) & 0x3;
			*destination++ = (value >> 0x1A) & 0x3;
			break;
		case 0x2:
			*destination++ = value & 0x7;
			*destination++ = (value >> 0x3) & 0x7;
			*destination++ = (value >> 0x6) & 0x7;
			*destination++ = (value >> 0x9) & 0x7;
			*destination++ = (value >> 0xC) & 0x7;
			*destination++ = (value >> 0xF) & 0x7;
			*destination++ = (value >> 0x12) & 0x7;
			*destination++ = (value >> 0x15) & 0x7;
			*destination++ = (value >> 0x18) & 0x7;
			break;
		case 0x3:
			*destination++ = value & 0xF;
			*destination++ = (value >> 0x4) & 0xF;
			*destination++ = (value >> 0x8) & 0xF;
			*destination++ = (value >> 0xC) & 0xF;
			*destination++ = (value >> 0x10) & 0xF;
			*destination++ = (value >> 0x14) & 0xF;
			*destination++ = (value >> 0x18) & 0xF;
			break;
		case 0x4:
			*destination++ = value & 0x1F;
			*destination++ = (value >> 0x5) & 0x1F;
			*destination++ = (value >> 0xA) & 0x1F;
			*destination++ = (value >> 0xF) & 0x1F;
			*destination++ = (value >> 0x14) & 0x1F;
			break;
		case 0x5:
			*destination++ = value & 0x7F;
			*destination++ = (value >> 0x7) & 0x7F;
			*destination++ = (value >> 0xE) & 0x7F;
			*destination++ = (value >> 0x15) & 0x7F;
			break;
		case 0x6:
			*destination++ = value & 0x1FF;
			*destination++ = (value >> 0x9) & 0x1FF;
			*destination++ = (value >> 0x12) & 0x1FF;
			break;
		case 0x7:
			*destination++ = value & 0x3FFF;
			*destination++ = (value >> 0xE) & 0x3FFF;
			break;
		case 0x8:
			*destination++ = value & 0xFFFFFFF;
			break;
		}
	}
}
