/*
	COMPRESS_SIMPLE16.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE16_H__
#define __COMPRESS_SIMPLE16_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIMPLE16
	--------------------------
*/
class ANT_compress_simple16 : public ANT_compress
{
protected:
	static long bits_to_use[];
	static long ints_packed_table[];
	static long simple16_shift_table[];
	static long simple16_mask_table[];

public:
	ANT_compress_simple16() {}
	virtual ~ANT_compress_simple16() {}

	virtual void pack(ANT_compressable_integer *source, uint32_t *dest, uint32_t mask_type, uint32_t num_to_pack);
	virtual long can_pack(ANT_compressable_integer *from, int mask_type, int pack_limit);
	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* __COMPRESS_SIMPLE16_H__ */
