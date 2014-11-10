/*
	COMPRESS_SIMPLE16_PACKED.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE16_PACKED_H__
#define __COMPRESS_SIMPLE16_PACKED_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIMPLE16_PACKED
	--------------------------
*/
class ANT_compress_simple16_packed : public ANT_compress
{
protected:
	// memory for packing compresser (i.e. determining optimal blocking)
	long *blocks_needed;
	unsigned char *masks;
	long blocks_length;
	
	static long ints_packed_table[];
	static long can_pack_table[];
	static long row_for_bits_needed[];
	static long invalid_masks_for_offset[];
	static long simple16_packed_shift_table[];

public:
	ANT_compress_simple16_packed()
	{
		blocks_needed = NULL;
		masks = NULL;
		blocks_length = 0;
	}
	virtual ~ANT_compress_simple16_packed()
	{
		delete blocks_needed;
		delete masks;
	}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif	/* __COMPRESS_SIMPLE16_PACKED_H__ */
