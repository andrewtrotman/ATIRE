/*
	COMPRESS_SIMPLE8B_PACKED.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE8B_PACKED_H__
#define __COMPRESS_SIMPLE8B_PACKED_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIMPLE8B_PACKED
	----------------------------------
*/
class ANT_compress_simple8b_packed : public ANT_compress
{
protected:
	// memory for packing compresser (i.e. determining optimal blocking)
	long *blocks_needed;
	unsigned char *masks;
	long blocks_length;
	
	static long ints_packed_table[];
	static long bits_used_table[];
	static long bits_to_use[];
	static long simple8b_packed_shift_table[];
	static long simple8b_packed_mask_table[];
private:
	virtual long can_pack(ANT_compressable_integer *from, uint32_t mask_type, uint32_t pack_limit);
	virtual void pack(ANT_compressable_integer *source, uint64_t *dest, uint32_t mask_type, uint32_t num_to_pack);
public:
	ANT_compress_simple8b_packed()
	{
		blocks_needed = NULL;
		masks = NULL;
		blocks_length = 0;
	}
	virtual ~ANT_compress_simple8b_packed()
	{
		delete blocks_needed;
		delete masks;
	}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* __COMPRESS_SIMPLE8B_PACKED_H__ */
