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

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_COMPRESS_SIMPLE9_PACKED::SIMPLE9_PACKED_TABLE[]
	-------------------------------------
	This is the simple-9-packed selector table (top 4 bits)
*/
ANT_compress_simple9_packed::ANT_compress_simple9_packed_lookup ANT_compress_simple9_packed::simple9_packed_table[] =
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
	ANT_COMPRESS_SIMPLE9_PACKED::SIMPLE9_PACKED_MASK_TABLE[]
	---------------------------------------------
	Number of bits required to compress each integer -- used to select the appropriate mask
*/
long ANT_compress_simple9_packed::simple9_packed_mask_table[] = {
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
14, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

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
	ANT_COMPRESS_SIMPLE9_PACKED::BITS_TO_USE[]
	-----------------------------------
	This is the number of bits that simple-9-packed will be used to store an integer of the given the number of bits in length
*/
long ANT_compress_simple9_packed::bits_to_use[] = 
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
	ANT_COMPRESS_SIMPLE9_PACKED::TABLE_ROW[]
	---------------------------------
	This is the row of the table to use given the number of integers we can pack into the word
*/
long ANT_compress_simple9_packed::table_row[] = 
{
0, 1, 2, 3, 4, 4, 5, 5, 
6, 6, 6, 6, 6, 7, 7, 7, 
7, 7, 7, 7, 7, 7, 7, 7, 
7, 7, 7, 8, 8
};

/*
	ANT_COMPRESS_SIMPLE9_PACKED::CAN_PACK()
	---------------------------------
	Try to pack integers according to the mask type
*/
long ANT_compress_simple9_packed::can_pack(ANT_compressable_integer *from, int mask_type, int pack_limit)
{
int offset = 0;
pack_limit = (pack_limit < ints_packed_table[mask_type]) ? pack_limit: ints_packed_table[mask_type];
while (offset < pack_limit)
	{
	if (*from++ >= 1 << simple9_packed_mask_table[offset + 28 * mask_type])
		return 0;
	offset++;
	}
return 1;
}

/*
	ANT_COMPRESS_SIMPLE9_PACKED:PACK()
	----------------------------
	Pack integers into word.
	TODO unroll this back into the compress() function.
*/
void ANT_compress_simple9_packed::pack(ANT_compressable_integer *source, uint32_t *dest, uint32_t mask_type, uint32_t num_to_pack)
{
	uint32_t offset = 0;
	int mask_type_offset = 28 * mask_type;
	uint32_t bits_to_shift = 0;
	*dest = 0;
	while (offset < num_to_pack)
		{
		*dest |= *(source + offset) << bits_to_shift;
		offset++;
        bits_to_shift = simple9_packed_shift_table[mask_type_offset + offset];
		}
	*dest = (*dest << 4) | mask_type;
}

/*
	ANT_COMPRESS_SIMPLE9_PACKED::COMPRESS()
	--------------------------------
*/
long long ANT_compress_simple9_packed::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
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
	pack(source, into, 8, 1);
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
masks[pos] = 8;

/* optimise from second-last value */
pos = source_integers - 2;

/* optimise packing masks to use */
while (pos >= 0)
{
	for (mask_type = 0; mask_type < 9; mask_type++)
	{
		num_to_pack = (pos + ints_packed_table[mask_type] > source_integers) ? source_integers - pos : ints_packed_table[mask_type];
		if (can_pack(source + pos, mask_type, num_to_pack) && pos + ints_packed_table[mask_type] <= source_integers)
		{
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
		}
		/* fail case: can't pack current block (i.e. x > 2^28) */
		else if (mask_type == 8) {
			return 0;
		}
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
	ANT_COMPRESS_SIMPLE9_PACKED::DECOMPRESS()
	----------------------------------
*/
void ANT_compress_simple9_packed::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
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
	bits = simple9_packed_table[8-row].bits;
	mask = simple9_packed_table[8-row].mask;

	switch (row)			// unwind the loop
		{
		case 0:			// 28 integers
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
		case 1:		// 14 integers
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
    case 2:		// 9 integers
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
		case 3:		// 7 integers
			*destination++ = value & mask;
			value >>= bits;
			*destination++ = value & mask;
			value >>= bits;
		case 4:		// 5 integers
			*destination++ = value & mask;
			value >>= bits;
		case 5:		// 4 integers
			*destination++ = value & mask;
			value >>= bits;
		case 6:		// 3 integers
			*destination++ = value & mask;
			value >>= bits;
		case 7:	// 2 integers
			*destination++ = value & mask;
			value >>= bits;
		case 8:	// 1 integer
			*destination++ = value;
		}
	}
}

